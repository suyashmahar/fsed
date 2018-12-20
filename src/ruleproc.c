//
// Created by suyas on 20-12-2018.
//

#include "ruleproc.h"

rule_t *parse_rule(char *rule) {
    assert(rule != NULL && "Null reference to rule.");

    size_t rulelen = strlen(rule);
    rule_t *result = calloc(1, sizeof(rule_t));
    result->orig = calloc(rulelen,sizeof(char));
    result->new = calloc(rulelen,sizeof(char));

    enum states {START = 0, ORIG = 1, NEW = 2} state = START;
    /* Current rule format: `/orig/new/' */
    int origptr = 0, newptr = 0;

    for (int rule_itr = 0; rule_itr < rulelen; rule_itr++) {
        switch (state) {
            case START: {
                if (rule[rule_itr] == SEP) {
                    state++;
                } else {
                    char *err = malloc(4096 * sizeof(char));
                    sprintf(err, "Rule should start with a `%c'\n", SEP);
                    error(err);
                }
                break;
            }
            case ORIG: {
                if (rule[rule_itr] == SEP) {
                    state++;
                    break;
                }
                result->orig[origptr++] = rule[rule_itr];
                break;
            }
            case NEW: {
                if (rule[rule_itr] == SEP) {
                    return result;
                }
                result->new[newptr++] = rule[rule_itr];
                break;
            }
            default: {
                error("Rule parser: Unknown state reached");
            }
        }
    }
    error("aldnfnaldfkn\n");
}