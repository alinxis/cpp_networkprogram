#include <iostream>
#include "error.h"

void errorf(bool condition , const char * msg){
    if(condition){
        perror(msg);
        exit(EXIT_FAILURE);
    }
}
void DEBUG(const char* msg,const char* funcname){
    std::cout<<"[DEBUG] "<<msg<<" --Function: "<<funcname<<std::endl;
}