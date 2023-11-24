#include <iostream>
#include "error.h"

void errorf(bool condition , const char * msg){
    if(condition){
        perror(msg);
        exit(EXIT_FAILURE);
    }
}
void Debugf(const char* msg,const char* funcname){
#ifdef DEBUG
    std::cout<<"[DEBUG] "<<msg<<" --Function: "<<funcname<<std::endl;
#endif
}