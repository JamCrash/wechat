
#include <stdio.h>
#include <string.h>
#include <unordered_set>
#include <string>
#include "print_msg.h"
#include "handler.h"
#include "dbg.h"

extern std::unordered_set<std::string> friend_list;

void handle_close(void* arg)
{
    handler_t* h = (handler_t*)arg;

    char name[h->arg_end - h->arg_start + 1];
    strncpy(name, h->arg_start, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';

    printf("connection to %s is closed\n", name);
    friend_list.erase()
}

void handle_connect(void* arg)
{
    handler_t* h = (handler_t*)arg;

    char buf[h->arg_end - h->arg_start + 1];
    strncpy(buf, h->arg_start, h->arg_end - h->arg_start);
    buf[sizeof(buf) - 1] = '\0';

    printf("%s request to connect. (y/n)\n", buf);
}

void handle_response(void* arg)
{
    handler_t* h = (handler_t*)arg;

    if(h->msglen != 1) {
        log_err("connection error");
        return ;
    }

    char name[h->arg_end - h->arg_start + 1];
    strncpy(name, h->arg_start, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';

    char msg_buf[h->msglen + 1];
    strncpy(msg_buf, h->parse_ptr, h->msglen);
    msg_buf[1] = '\0';
 
    if(strncmp(msg_buf, "y", 1) == 0) {
        printf("connection to %s established\n", name);
        friend_list.insert(std::string(name));
    }
    else if(strncmp(msg_buf, "n", 1) == 0) {
        printf("connection to %s refused\n", name);
    }
    else {
        log_err("connection error");
    }
}

void handle_send(void* arg)
{
    handler_t* h = (handler_t*)arg;

    char name[h->arg_end - h->arg_start + 1];
    strncpy(name, h->arg_start, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';

    char msg_buf[h->msglen + 1];
    strncpy(msg_buf, h->parse_ptr, h->msglen);
    msg_buf[h->msglen] = '\0';

    printf("message from %s: %s", name, msg_buf);
}
