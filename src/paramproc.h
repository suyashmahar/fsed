//
// Created by suyas on 19-12-2018.
//

#ifndef FSED_PARAMPROC_H
#define FSED_PARAMPROC_H

#include <stdint-gcc.h>

#define NO_VERBOSE (0)
#define VERBOSE_L1 (1)
#define VERBOSE_L2 (2)
#define VERBOSE_L3 (3)

typedef struct configs {
    uint8_t verbose;
    char *targetfile;
    char *rule;
} configs_t;

/* Processes arguments of the program and returns the number of parameters processed */
int procconfigs(int argc, char *argv[], configs_t *progconfigs);

void printconfigs(configs_t *progconfigs);

#endif //FSED_PARAMPROC_H
