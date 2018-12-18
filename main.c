#include "main.h"

const size_t long_sz = sizeof(long);

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

void getdata(pid_t child, long addr, char *str, int len) {
    char *laddr;
    int i;
    size_t j;

    union u {
        long val;
        char chars[long_sz];
    }data;

    i = 0;
    j = len / long_sz;
    laddr = str;
    while(i < j) {
        data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * long_sz, NULL);
        memcpy(laddr, data.chars, long_sz);
        ++i;
        laddr += long_sz;
    }
    j = len % long_sz;
    if(j != 0) {
        data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * long_sz, NULL);
        memcpy(laddr, data.chars, j);
    }
    str[len] = '\0';
}

void putdata(pid_t child, long addr, char *str, int len) {
    char *laddr;
    int i;
    size_t j;
    union u {
        long val;
        char chars[long_sz];
    } data;

    i = 0;
    j = len / long_sz;
    laddr = str;
    while(i < j) {
        memcpy(data.chars, laddr, long_sz);
        ptrace(PTRACE_POKEDATA, child, addr + i * long_sz, data.val);
        ++i;
        laddr += long_sz;
    }
    j = len % long_sz;
    if(j != 0) {
        memcpy(data.chars, laddr, j);
        ptrace(PTRACE_POKEDATA, child, addr + i * long_sz, data.val);
    }
}

int main() {
    pid_t child;
    child = fork();
    bool insyscall = false;

    fdentry_t **fdtbl = malloc(FD_MAX * sizeof(fdentry_t));
    for (int i = 0; i < FD_MAX; i++) {
        fdtbl[i] = NULL;
    }

    if (child == 0) { /* Inside child */
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    } else { /* inside parent */
        long orig_rax;
        long params[3];
        int status;
        char *str, *laddr;
        int toggle = 0;
        while(1) {
            wait(&status);

            if(WIFEXITED(status)) {
                break;
            }
            orig_rax = ptrace(PTRACE_PEEKUSER, child, long_sz * ORIG_RAX, NULL);

            if (!(insyscall^=1)) { /* false */
                /* Switch between sys calls */
                switch (orig_rax) {
                    case SYS_OPENAT: {
                       break;
                    }
                    case SYS_READ:
                        break;
                    case SYS_WRITE: {
                        if(toggle == 0) {
                            toggle = 1;
                            params[0] = ptrace(PTRACE_PEEKUSER, child, long_sz * RDI, NULL);
                            params[1] = ptrace(PTRACE_PEEKUSER, child, long_sz * RSI, NULL);
                            params[2] = ptrace(PTRACE_PEEKUSER, child, long_sz * RDX, NULL);

                            str = (char *)calloc((size_t)(params[2]+1),sizeof(char));
                            getdata(child, params[1], str, (int)params[2]);
                            reverse(str);
                            putdata(child, params[1], str, (int)params[2]);
                        }
                        else {
                            toggle = 0;
                        }
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
                    case SYS_READ:
                    case SYS_WRITE:
                        break;
                    case SYS_OPENAT: {
                        struct user_regs_struct regs;
                        ptrace(PTRACE_GETREGS, child, NULL, &regs);

                        int fd = (int)regs.rax;

                        params[0] = ptrace(PTRACE_PEEKUSER, child, long_sz * RDI, NULL); /* const char __user * filename */
                        params[1] = ptrace(PTRACE_PEEKUSER, child, long_sz * RSI, NULL); /* int flags */
                        params[2] = ptrace(PTRACE_PEEKUSER, child, long_sz * RDX, NULL); /* umode_t mode */

                        if (fdtbl[fd] == NULL) {
                            fdtbl[fd] = malloc(sizeof(fdentry_t));
                        }

                        fdtbl[fd]->fpath = (char*)calloc(FNAME_MAX+1,sizeof(char));

                        getdata(child, params[1], fdtbl[fd]->fpath, FNAME_MAX);

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
            printf("%3d: %3d -> %s\n", i, fdtbl[i]->fd, fdtbl[i]->fpath);
        }
    }
    return 0;
}
