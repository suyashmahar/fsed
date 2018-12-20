//
// Created by suyas on 20-12-2018.
//

#ifndef FSED_RULEPROC_H
#define FSED_RULEPROC_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "helper.h"

typedef struct rule {
    char *orig;
    char *new;
} rule_t;

#define SEP '/'

rule_t *parse_rule(char *rule);

#endif //FSED_RULEPROC_H
