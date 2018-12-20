#include "paramproc.h"
#include "helper.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/* Processes arguments of the program and returns the number of parameters processed */
int procconfigs(int argc, char *argv[], configs_t *progconfigs) {
    progconfigs->verbose = NO_VERBOSE;
    int param_proc_count = 0;
    for (int i = 1; i < argc; i++) {
        /* Process `--args' first to avoid parsing tracee's arguments */
        if (!strcmp(argv[i], "--args")) {
            if (i+1 < argc) {
                return param_proc_count+1;
            } else {
                error("No program specified for option `--args', see --help for more details.\n");
            }
        } else if (!strcmp(argv[i], "--help")) {
            printf("fsed is pretty sad\n");
            exit(0);
        }  else if (!strcmp(argv[i], "--version")) {
            printf("fsed is pretty sad at v0.1\n");
            exit(0);
        } else if (!strcmp(argv[i], "-f")) {
            if (i+1 < argc) {
                progconfigs->targetfile = argv[i+1];
            } else {
                error("No target file specified for option `-f', see --help for more details.\n");
            }
        } else if (!strcmp(argv[i], "-r")) {
            if (i+1 < argc) {
                progconfigs->rule = argv[i+1];
            } else {
                error("No rule specified for option `-r', see --help for more details.\n");
            }
        } else if (!strcmp(argv[i], "-v")) {
            progconfigs->verbose = VERBOSE_L1;
        } else if (!strcmp(argv[i], "-vv")) {
            progconfigs->verbose = VERBOSE_L2;
        } else if (!strcmp(argv[i], "-vvv")) {
            progconfigs->verbose = VERBOSE_L3;
        }
        param_proc_count++;
    }
    error("No program passed in arguments, use --help for details on how to use fsed.\n");
    return -1;
}

void printconfigs(configs_t *progconfigs) {
    printf("configs:\n");
    printf("targetfile: %s\n", progconfigs->targetfile);
    printf("rule: %s\n", progconfigs->rule);
    printf("verbosity: %d\n", progconfigs->verbose);
}

