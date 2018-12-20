//
// Created by suyas on 18-12-2018.
//

#ifdef __x86_64__

#ifndef FSED_MAIN_H
#define FSED_MAIN_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/ptrace.h>
#include <sys/reg.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <fcntl.h>
#include <linux/limits.h>

/* x86 sys calls */

#define SYS_READ   0
#define SYS_WRITE  1
#define SYS_OPEN   2
#define SYS_OPENAT 257

/* Limits */
#define FD_MAX 100 /* Maximum number of file descriptor that can be stored */
#define FNAME_MAX 1000 /* Max filename in chars */
/* For storing information of a file descriptor */
typedef struct fdentry {
    int fd;
    char *fpath;
    int flags;
    int mode;
    bool marked;
} fdentry_t;


#endif //FSED_MAIN_H

#else
#error "Unknown architecture fsed is currently implemented only for x86_64"
#endif
