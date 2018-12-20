//
// Created by suyas on 19-12-2018.
//

#ifndef FSED_HELPER_H
#define FSED_HELPER_H

#include <stdio.h>
#include <sys/reg.h>
#include "ruleproc.h"

const size_t long_sz;

/* Valid only for x86 */
#ifdef __x86_64__
const unsigned int reg_order[6];
#else
#error "Unknown architecture"
#endif

void error(char *errstr);

#endif //FSED_HELPER_H
