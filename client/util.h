
#ifndef UTIL_H
#define UTIL_H

#include <pthread.h>

#define SMALLSIZE   64
#define MAXNAMELEN  8

#define makethread(tid_ptr, fn, arg)    \
({  \
    int err;    \
    pthread_attr_t attr;    \
    err = pthread_attr_init(&attr); \
    if(err == 0){   \
        err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);  \
        if(err == 0) {  \
            err = pthread_create(tid_ptr, &attr, fn, arg);   \
        }   \
        pthread_attr_destroy(&attr);    \
    }   \
    err;    \
})

int make_socket_nonblock(int fd);
int check_in(int fd);

#endif  /* UTIL_H */
