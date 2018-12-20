#include "main.h"
#include "paramproc.h"
#include "helper.h"
#include "childhelper.h"
#include "ruleproc.h"


int main(int argc, char* argv[]) {
    configs_t progconfigs;
    int push = procconfigs(argc, argv, &progconfigs);
    printf("verbose: %d\n", progconfigs.verbose);
    if (progconfigs.verbose > NO_VERBOSE)
        printconfigs(&progconfigs);

    rule_t *rule = parse_rule(progconfigs.rule);
    if (progconfigs.verbose > VERBOSE_L1)
        printf("parsed rule: %s -> %s\n", rule->orig, rule->new);

    pid_t child;

    child = fork();
    if (progconfigs.verbose > VERBOSE_L2)
        printf("child pid: %d\n", ((int)child));

    bool beforesyscall = false;

    fdentry_t **fdtbl = malloc(FD_MAX * sizeof(fdentry_t));
    for (int i = 0; i < FD_MAX; i++) {
        fdtbl[i] = NULL;
    }

    if (child == 0) { /* Inside child */
        ptrace(PTRACE_TRACEME/*, 0, NULL, NULL*/);
        int margc = argc-push;

        char *args [margc];

        /* Skip the program name while converting to arguments.
         * Input format: program_path [args]*/
        memcpy(args, argv+push+1, (margc-1) * sizeof(char*));

        args[margc] = NULL; /* NULL terminate arg arr */
        execvp(args[0], args);
    } else { /* inside parent */
        long orig_rax;
        long *params;
        int status;
        char *str, *laddr;
        int toggle = 0;
        while(1) {
            wait(&status);

            if(WIFEXITED(status)) {
                break;
            }
            orig_rax = ptrace(PTRACE_PEEKUSER, child, long_sz * ORIG_RAX, NULL);

            if (!(beforesyscall^=1)) { /* false */
                /* Switch between sys calls */
                switch (orig_rax) {
                    case SYS_OPENAT:
                        break;
                    case SYS_READ:{
                        break;
                    }
                    case SYS_WRITE: {
                        params = getparams(child, 3);

                        str = (char *)calloc((size_t)(params[2]+1),sizeof(char));
                        getdata(child, params[1], str, (int)params[2]);
                        reverse(str);
                        putdata(child, params[1], str, (int)params[2]);

                        free(params);
                        break;
                    }
                    default: {
                        break;
                    }
                }
            } else { /* true */
                /* Switch between sys calls */
                switch (orig_rax) {
                    case SYS_OPEN: {
                        fprintf(stderr, "This syscall is not yet implemented.\n");
                        break;
                    }
                    case SYS_READ: {
                        params = getparams(child, 3);
                        int fd = (int)params[0];
                        if (fdtbl[fd]->marked) {
                            str = (char *)calloc((size_t)(params[2]+1),sizeof(char));
                            getdata(child, params[1], str, (int)params[2]);
                            reverse(str);
                            putdata(child, params[1], str, (int)params[2]);
                        }

                        free(params);
                        break;
                    }
                    case SYS_WRITE:
                        break;
                    case SYS_OPENAT: {
                        struct user_regs_struct regs;
                        ptrace(PTRACE_GETREGS, child, NULL, &regs);

                        int fd = (int)regs.rax;
                        params = getparams(child, 3);

                        if (fdtbl[fd] == NULL) {
                            fdtbl[fd] = malloc(sizeof(fdentry_t));
                        }

                        fdtbl[fd]->fpath = (char*)calloc(FNAME_MAX+1,sizeof(char));

                        getdata(child, params[1], fdtbl[fd]->fpath, FNAME_MAX);
                        if (0 == strcmp(fdtbl[fd]->fpath, progconfigs.targetfile)) {
                            fdtbl[fd]->marked = true;
                        } else { /* Table entry might already be written */
                            fdtbl[fd]->marked = false;
                        }

                        printf("fd: %3d -> %s\n", fd, fdtbl[fd]->fpath);
                        fdtbl[fd]->flags = (int)params[1];
                        fdtbl[fd]->mode = (int)params[2];
                        break;
                    }
                    default:
                        break;
                }
            }
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
        }
    }
    printf("Exiting...\n");
    for (int i = 0; i < FD_MAX; i++) {
        if (fdtbl[i] != NULL) {
            printf("%3d: [%d]%3d -> '%s'\n", i, fdtbl[i]->marked, fdtbl[i]->fd, fdtbl[i]->fpath);
        }
    }
    return 0;
}
