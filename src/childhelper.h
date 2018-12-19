//
// Created by suyas on 19-12-2018.
//

#ifndef FSED_CHILDHELPER_H
#define FSED_CHILDHELPER_H

#include <sys/types.h>

long* getparams(pid_t child, unsigned int paramcnt);

void getdata(pid_t child, long addr, char *str, int len);

void putdata(pid_t child, long addr, char *str, int len);

#endif //FSED_CHILDHELPER_H
