#pragma once
#include <algorithm>
#include <cerrno>
#include <condition_variable>
#include <cstddef>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <memory>
#include<cassert>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <stdexcept>
#include <type_traits>



template <typename T, typename = void>
struct HasDefaultInitializer : std::false_type {};

// 特化以包括所有标量类型和 POD 类型
template <typename T>
struct HasDefaultInitializer<T,
        std::enable_if_t<std::is_scalar<T>::value || (std::is_standard_layout<T>::value && std::is_trivial<T>::value)>> : std::true_type {};


template <typename T>
class Buffer {
    //编译检查，为了防止std::copy造成隐式的错误，要求泛型的类必须是POD类型或者 显示提供了拷贝构造函数的类
#ifndef __UnSafe__
    static_assert(
            std::is_standard_layout<T>::value&& std::is_trivial<T>::value,
            "Type must be trivial and standard layout or provide a non-trivial copy constructor.");
#endif
public:

    //仅使用大小初始化缓冲区
    Buffer(size_t size, size_t capacity, std::function<void(T*)>initializer = nullptr)
            : _size(size),
              _capacity(std::max(size, capacity)),
              _defaultInitializer(initializer) {

        if (capacity > 0) {
            _data = std::make_unique<T[]>(capacity);
        }
        else {
            _data = nullptr;
        }
        //如果没有提供初始化函数，并且泛型的类型可以被默认初始化，就提供默认的初始化函数
        if (initializer == nullptr && HasDefaultInitializer<T>::value) {
            _defaultInitializer = [](T* value) { std::memset(value, 0, sizeof(value)); };
        }
        Initial(0,size);
    }
    explicit Buffer(size_t size) : Buffer(size, size) {}
    Buffer() :Buffer(0) {}

    //将已存在的缓冲区进行接管
    Buffer(const T* data, size_t size, size_t capacity) : Buffer(size, capacity) {
        std::copy(data, data + size, _data.get());
    }
    Buffer(const T* data, size_t size) : Buffer(data, size, size) {}

    ~Buffer() = default;

    //禁止赋值构造，这会严重造成性能落后
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    //移动构造函数
    Buffer(Buffer&& buffer)  noexcept :
            _size(buffer.size()),
            _capacity(buffer.capacity()),
            _data(std::move(buffer._data)) {
        assert(IsConsistent());//检查内存是否有效
        buffer._size = 0;
        buffer._capacity = 0;
    }
    //移动赋值函数
    Buffer& operator=(Buffer&& other) {
        assert(IsConsistent());
        if (&other == this) return *this; //如果是自己拷贝自己，就什么都不做

        //先释放已有的资源
        _data.reset();
        //资源转移
        _data = std::move(other._data);
        _size = other._size;
        _capacity = other._capacity;
        //清空 other 对象
        other._size = 0;
        other._capacity = 0;
        return *this;
    }

    // 非常量版本的operator[]，允许修改元素
    T& operator[](size_t index) {
        //if (index >= _size) throw std::out_of_range("Index out of range.");
        return _data[index];
    }

    // 常量版本的operator[]，仅用于访问元素
    const T& operator[](size_t index) const {
        //if (index >= _size) throw std::out_of_range("Index out of range.");
        return _data[index];
    }
    //这是一个安全函数，当索引无效时，它不会抛出异常，而是返回一个空
    std::optional<std::reference_wrapper<T>> at(size_t index) noexcept {
        if (index >= _size) return std::nullopt;
        return std::optional<std::reference_wrapper<T>>(std::ref(_data[index]));
    }

    void append(const T* data, size_t num_of_data) {
        std::unique_lock<std::mutex> lock(mutex); // 加锁以保证线程安全
        size_t new_size = _size + num_of_data;
        if (new_size > _capacity) {
            Expand(std::max(new_size, _size + _size / 2));
        }
        // 保证在复制数据之前和之后，对象的状态都是一致的
        assert(IsConsistent());
        std::copy(data, data + num_of_data, _data.get() + _size);
        _size = new_size;
        assert(IsConsistent());
        cv.notify_one(); // 如果有等待的线程，通知它们
    }

    void write_to_head(const T* data,size_t num_of_data ) {
        std::unique_lock<std::mutex> lock(mutex);
        assert(IsConsistent());
        size_t old_size=_size;
        _size=0;
        Append(data,num_of_data);
        if(num_of_data < old_size) {
            Initial(_size,old_size-num_of_data);
        }
        cv.notify_one();
    }

    void clear() {
        std::unique_lock<std::mutex> lock(mutex);
        if(_defaultInitializer!=nullptr) Initial();
        _size=0;
        assert(IsConsistent());
        cv.notify_one();
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(mutex);
        return _size==0;
    }


    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return _size;
    }
    size_t capacity() const {
        std::lock_guard<std::mutex> lock(mutex);
        return _capacity;
    }
    //直接获取裸指针会造成对内存的破坏，但是某些情况又需要,由于扩容机制，这会导致指针失效
    T* Data() {
        std::lock_guard<std::mutex> lock(mutex);
        return _data.get();
    }
    // 获取指向缓冲区数据的常量指针（只读）
    const T* Data() const {
        std::lock_guard<std::mutex> lock(mutex);
        return _data.get();
    }

    //对缓冲区进行控制，size只能缩小,capacity只可以扩大
    void Resize(size_t newsize) {
        std::unique_lock<std::mutex> lock(mutex);
        if (newsize == _capacity) return;
        if (newsize > _capacity) {
            Expand(newsize);
            return;
        }
        for (int i = newsize; i < _size; i++) {
            _defaultInitializer(&_data[i]);
        }
        _size=newsize;
        cv.notify_one();
    }
    //对缓冲区进行扩张
    void Expand(size_t newCapacity) {
        std::unique_lock<std::mutex> lock(mutex);
        if (newCapacity <= _capacity) return;
        // 创建新的内存块
        std::unique_ptr<T[]> newData = std::make_unique<T[]>(newCapacity);
        // 复制现有数据到新的内存块
        if (_data != nullptr) {
            std::copy(_data.get(), _data.get() + _size, newData.get());
        }
        // 更新内部指针和容量
        _data = std::move(newData);
        _capacity = newCapacity;
        cv.notify_one();
    }


    void SetInitializer(std::function<void(T*)> initializer) { _defaultInitializer = std::move(initializer); }
    bool Initial(size_t begin,size_t len) {
        std::unique_lock<std::mutex> lock(mutex);
        if (_defaultInitializer == nullptr) return false;
        size_t end=std::min(begin+len,_capacity);
        for (size_t i = begin; i < end; i++) {
            _defaultInitializer(&_data[i]);
        }
        cv.notify_one();
        return true;

    }
    bool Initial_s(size_t begin=0) {
        return Initial(begin,_capacity);
    }

    void initializer(T* item) {
        _defaultInitializer(item);
    }

#ifdef DEBUG
    void show(std::string info=""){
        std::cout<<"================================"<<info<<"=============================="<<std::endl;
        std::cout<<"[buffer]: ";
        for (int i = 0; i < _capacity; i++) {
            std::cout << _data[i] << " ";
        }
        std::cout<<std::endl;
        std::cout << "buffer size: " << _size << " buffer capacity " << _capacity << std::endl;
    }
#endif

private:
    bool IsConsistent() const {
        if (_size > _capacity) return false;
        if (_capacity > 0 && !_data) return false;
        return true;
    }



private:
    size_t _size;
    size_t _capacity;
    std::unique_ptr<T[]>  _data;

    std::function<void(T*)> _defaultInitializer; //默认的初始化函数

    mutable std::mutex mutex;
    mutable std::shared_mutex rw_mutex;
    std::condition_variable cv;
};