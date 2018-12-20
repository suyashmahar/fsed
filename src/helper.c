//
// Created by suyas on 19-12-2018.
//

#include "helper.h"
#include "ruleproc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t long_sz = sizeof(long);
const unsigned int reg_order[6] = {RDI, RSI, RDX, R10, R8, R9};

void error(char *errstr) {
    fprintf(stderr, "%s", errstr);
    exit(1);
}

void testalloc(void *ptr) {
    if (ptr == NULL) {
        error("Memory allocation failed.");
    }
}

