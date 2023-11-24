//
// Created by King on 2023/11/3.
//

#ifndef CPPFRAME_ERROR_H
#define CPPFRAME_ERROR_H

#include <cstdio>
#include <cstdlib>

void errorf(bool condition , const char * msg);
void Debugf(const char* msg, const char* funcname);
#endif //CPPFRAME_ERROR_H
