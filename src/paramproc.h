//
// Created by suyas on 19-12-2018.
//

#ifndef FSED_PARAMPROC_H
#define FSED_PARAMPROC_H

typedef struct configs {
    char *targetfile;
    char *rule;
} configs_t;

/* Processes arguments of the program and returns the number of parameters processed */
int procconfigs(int argc, char *argv[], configs_t *progconfigs);

#endif //FSED_PARAMPROC_H
