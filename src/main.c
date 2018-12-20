#include "main.h"
#include "paramproc.h"
#include "helper.h"
#include "childhelper.h"
#include "ruleproc.h"

/* Count number of from string occurrences */
size_t countFreq(const char *pat, const char *txt) {
    int M = (int)strlen(pat);
    int N = (int)strlen(txt);
    int res = 0;

    /* A loop to slide pat[] one by one */
    for (int i = 0; i <= N - M; i++)
    {
        /* For current index i, check for
           pattern match */
        int j;
        for (j = 0; j < M; j++)
            if (txt[i+j] != pat[j])
                break;

        // if pat[0...M-1] = txt[i, i+1, ...i+M-1]
        if (j == M)
        {
            res++;
            j = 0;
        }
    }
    return (size_t)res;
}

int *getpos(const char *pat, const char *txt, int count) {
    int M = (int)strlen(pat);
    int N = (int)strlen(txt);
    int res = 0;

    int *pos = malloc(count*sizeof(int));
    int pos_itr = 0;
    /* A loop to slide pat[] one by one */
    for (int i = 0; i <= N - M; i++)
    {
        /* For current index i, check for
           pattern match */
        int j;
        for (j = 0; j < M; j++)
            if (txt[i+j] != pat[j])
                break;

        // if pat[0...M-1] = txt[i, i+1, ...i+M-1]
        if (j == M)
        {
            pos[pos_itr++] = i;
            j = 0;
        }
    }
    return pos;
}

int replace(FILE *fp, char *buf, char *resultbuf, int buflen, char *from, char *to) {
    testalloc(resultbuf);

    fread(resultbuf, buflen*sizeof(unsigned char), 1, fp);

    int *pos = getpos(from, buf, (int)countFreq(from, buf));
    int poslen = (int)countFreq(from, buf);

    size_t tolen = strlen(to);
    int pos_itr = 0, result_itr = 0;
    for (int i = 0; i < buflen; i++) {
        if (i == pos[pos_itr]) {
            for (int to_itr = 0; (to_itr < tolen) && (i+to_itr < buflen); to_itr++) {
                resultbuf[result_itr++] = to[to_itr];
            }
            pos_itr = pos_itr < poslen-1 ? pos_itr+1 : pos_itr;
            i += tolen-1;
        } else {
            resultbuf[result_itr++] = buf[i];
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    FILE *targetfp;

    configs_t progconfigs;

    int push = procconfigs(argc, argv, &progconfigs);

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
                            str = (char *)calloc((size_t)(params[2]+1+(rule->new)-(rule->orig)),sizeof(char));
                            int strlen = (int)params[2];

                            getdata(child, params[1], str, strlen);

                            char *resultbuf = calloc((size_t)strlen, sizeof(unsigned char));
                            replace(targetfp, str, resultbuf, strlen, rule->orig, rule->new);
                            if (progconfigs.verbose == VERBOSE_L3) {
                                printf("Replaced `%s' with `%s'\n", str, resultbuf);
                            }
                            putdata(child, params[1], resultbuf, strlen);

                            /* Change the size of buf */
                            ptrace(PTRACE_POKEUSER, child, sizeof(long)*RDX, params[2]+1+(rule->new)-(rule->orig));
                        }

                        free(params);
                        break;
                    }
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
                            targetfp = fopen(fdtbl[fd]->fpath, "rb");
                            if (progconfigs.verbose > NO_VERBOSE)
                                printf("Target fd(%d) for %s found.\n", fd, progconfigs.targetfile);
                        } else { /* Table entry might already be written */
                            fdtbl[fd]->marked = false;
                        }

                        if (progconfigs.verbose == VERBOSE_L3)
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
    if (progconfigs.verbose == VERBOSE_L3) {
        printf("Exiting, listing open file descriptors...\n");
        for (int i = 0; i < FD_MAX; i++) {
            if (fdtbl[i] != NULL) {
                printf("%3d: [%d]%3d -> '%s'\n", i, fdtbl[i]->marked, fdtbl[i]->fd, fdtbl[i]->fpath);
            }
        }
    }
    return 0;
}
