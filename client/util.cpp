
#include <fcntl.h>
#include "util.h"

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
