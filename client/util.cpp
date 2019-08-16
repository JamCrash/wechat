
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "util.h"
#include "dbg.h"

int make_socket_nonblock(int fd)
{
    int state;
    state = fcntl(fd, F_GETFD, 0);
    if(state < 0) {
        return -1;
    }
    state |= O_NONBLOCK;
    return fcntl(fd, F_SETFD, state);
}

int Read(int fd, char* buf, int bufsize, int timeout)
{
    alarm(timeout);
    return read(fd, buf, bufsize);
}

void sig_alrm(int signo)
{
    //nothing to do
}

//TODO
//死循环
int check_in(int fd)
{
    struct sigaction act, oact;
    sigemptyset(&act.sa_mask);
    act.sa_handler = sig_alrm;
    act.sa_flags = SA_INTERRUPT;

    if(sigaction(SIGALRM, &act, &oact) != 0) {
        return -1;
    }

    char buf[SMALLSIZE];
    int n;
    while(n = Read(fd, buf, SMALLSIZE, 3)) {
        if(n < 0) {
            if(errno == EINTR) {
                continue;
            }
            else {
                log_err("unknown error occur");
                return -1;
            }
        }
        if(n == 0) {
            log_err("server terminate previously");
            return -1;
        }
        write(STDOUT_FILENO, buf, n);
        buf[n] = '\0';
        if(strncmp(buf, "exit", 4) == 0) {
            return -1;
        }
        if(strncmp(buf, "Register successfully", 21) == 0) {
            break;
        }
again:
        n = Read(STDIN_FILENO, buf, MAXNAMELEN + 1, 100);
        if(n == 0) {
            printf("Please input your user name: \n");
            goto again;
        }
        if(n < 0) {
            if(errno == EINTR) {
                printf("Timeout exit\n");
                return -1;
            }
            else {
                log_err("unknown error occur");
                return -1;
            }
        }
        write(fd, buf, n);
    }   /* while */

    if(sigaction(SIGALRM, &oact, NULL) != 0) {
        return -1;
    }
    
    return 0;
}