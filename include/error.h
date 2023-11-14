//
// Created by King on 2023/11/3.
//

#ifndef CPPFRAME_ERROR_H
#define CPPFRAME_ERROR_H

#include <cstdio>
#include <cstdlib>

void errorf(bool condition , const char * msg){
    if(condition){
        perror(msg);
        exit(EXIT_FAILURE);
    }
}
#endif //CPPFRAME_ERROR_H
