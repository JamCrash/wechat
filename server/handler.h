
#ifndef HANDLER_H
#define HANDLER_H

#define MAXSIZE 1024

struct handler_t {
    handler_t(int infd)
        : fd(infd), remain_size(MAXSIZE)
        , parse_byte(0), state(0), method(0)
        , msglen(0) { 
        cur_pos = parse_ptr = msg_bufer;
    }
    int     fd;
    char    msg_bufer[MAXSIZE];
    char*   parse_ptr;
    char*   cur_pos;
    int     parse_byte;
    int     remain_size;
    int     state;
    int     method;
    char*   method_start;
    char*   method_end;
    char*   arg_start;
    char*   arg_end;
    int     msglen;
    char*   msglen_start;
    char*   msglen_end;
};

#endif  /* HANDLER_H */