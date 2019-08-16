
#ifndef HANDLER_H
#define HANDLER_H

#define BUFSIZE 1024
#define HEADLEN 40

struct handler_t {
    handler_t(int infd): fd(infd), head_remain_size(HEADLEN), state(0)
    {
        cur_pos = parse_ptr = bufer;
    }
    int     fd;
    char    bufer[BUFSIZE];
    char*   cur_pos;
    char*   parse_ptr;
    int     parse_byte;
    int     head_remain_size;
    int     state;
    char*   method_start;
    char*   method_end;
    int     method;
    char*   arg_start;
    char*   arg_end;
    char*   msglen_start;
    char*   msglen_end;
    int     msglen;
};

#endif  /* HANDLER_H */
