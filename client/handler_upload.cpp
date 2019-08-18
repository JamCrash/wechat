
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "handle_upload.h"

void send_msg(int fd, void* arg)
{
    pthread_t tid = *(pthread_t*)arg;

    char bufer[BUFSIZE];
    char* ret;
    while(ret = fgets(bufer, BUFSIZE, stdin)) {
        struct parser p;
        parse_method(bufer, BUFSIZ, &p);
        switch(p.method) {
            case UPLOAD_CLOSE:

            case UPLOAD_CONNECT:
            
            case UPLOAD_SEND:

            case UPLOAD_LIST:
            
            case UPLOAD_RESPONSE:

            default:
                //UPLOAD_UNKNOWN

        }
    }
}

void parse_method(char* bufer, size_t size, struct parser* p)
{
    char* ptr = bufer;
    size_t n1 = 0;
    size_t n2 = 0;
    while(n1 < size && *(bufer + n1) == ' ') ++n1;
    n2 = n1;
    while(n2 < size && *(bufer + n2) != ' ') ++n2;
    if(n2 == size) {
        p->method = UPLOAD_UNKNOWN;
        return;
    }
    p->arg_start = (bufer + n2);
    char buf[n2 - n1 + 1];
    strncpy(buf, (buf + n1), n2 - n1);
    buf[n2 - n1] = '\0';
    if(strncmp("send", buf, n2 - n1) == 0) {
        p->method = UPLOAD_SEND;
    }
    else if(strncmp("response", buf, n2 - n1) == 0) {
        p->method = UPLOAD_RESPONSE;
    }
    else if(strncmp("close", buf, n2 - n1) == 0) {
        p->method = UPLOAD_CLOSE;
    }
    else if(strncmp("connect", buf, n2 - n1) == 0) {
        p->method = UPLOAD_CONNECT;
    }
    else if(strncmp("list", buf, n2 - n1) == 0) {
        p->method = UPLOAD_LIST;
    }
    else {
        p->method = UPLOAD_UNKNOWN;
    }
}
