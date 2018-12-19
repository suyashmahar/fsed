//
// Created by suyas on 19-12-2018.
//

#include "helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t long_sz = sizeof(long);
const unsigned int reg_order[6] = {RDI, RSI, RDX, R10, R8, R9};

void error(char *errstr) {
    fprintf(stderr, "%s", errstr);
    exit(1);
}

void reverse(char *str) {
    int i;
    size_t j;
    char temp;
    for(i = 0, j = strlen(str) - 2;
        i <= j; ++i, --j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}
