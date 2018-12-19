//
// Created by suyas on 19-12-2018.
//

#include <sys/ptrace.h>
#include <stdlib.h>
#include <string.h>

#include "helper.h"
#include "childhelper.h"

long* getparams(pid_t child, unsigned int paramcnt) {
    long *params = malloc(paramcnt*sizeof(long));

    for (unsigned int i = 0; i < paramcnt; i++) {
        params[i] = ptrace(PTRACE_PEEKUSER, child, long_sz * reg_order[i], NULL);
    }

    return params;
}

void getdata(pid_t child, long addr, char *str, int len) {
    char *laddr;
    int i;
    size_t j;

    union u {
        long val;
        char chars[long_sz];
    }data;

    i = 0;
    j = len / long_sz;
    laddr = str;
    while(i < j) {
        data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * long_sz, NULL);
        memcpy(laddr, data.chars, long_sz);
        ++i;
        laddr += long_sz;
    }
    j = len % long_sz;
    if(j != 0) {
        data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * long_sz, NULL);
        memcpy(laddr, data.chars, j);
    }
    str[len] = '\0';
}

void putdata(pid_t child, long addr, char *str, int len) {
    char *laddr;
    int i;
    size_t j;
    union u {
        long val;
        char chars[long_sz];
    } data;

    i = 0;
    j = len / long_sz;
    laddr = str;
    while(i < j) {
        memcpy(data.chars, laddr, long_sz);
        ptrace(PTRACE_POKEDATA, child, addr + i * long_sz, data.val);
        ++i;
        laddr += long_sz;
    }
    j = len % long_sz;
    if(j != 0) {
        memcpy(data.chars, laddr, j);
        ptrace(PTRACE_POKEDATA, child, addr + i * long_sz, data.val);
    }
}