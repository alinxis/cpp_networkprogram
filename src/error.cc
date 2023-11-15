#include "error.h"

void errorf(bool condition , const char * msg){
    if(condition){
        perror(msg);
        exit(EXIT_FAILURE);
    }
}