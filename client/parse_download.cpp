
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "print_msg.h"
#include "parse_download.h"
#include "handler.h"

//note: fd isn't nonblock
void* read_info(void* arg)
{
    handler_t* h = (handler_t*)arg;

    int n;
    int parse_state;
    while(1) {
        n = read(h->fd, h->cur_pos, h->head_remain_size);
        if(n == 0) {
            //TODO
        }
        if(n < 0) {
            //TODO
            //error
        } 
        h->head_remain_size -= n;
        h->cur_pos += n;
        h->parse_byte += n;
        if(h->head_remain_size > 0)
            continue;

        //h->head_remain_size == 0
        parse_state = parse_download_head(h);
        if(parse_state != PARSE_DOWNLOAD_OK) {
            //TODO
            //error
        }

        //parse_download_ok
        print_msg(arg);
    }
}

void print_msg(void* arg)
{
    handler_t* h = (handler_t*)arg;

    if(h->method == DOWNLOAD_CLOSE) {
        handle_close(arg);
    }
    else if(h->method == DOWNLOAD_CONNECT) {
        handle_connect(arg);
    }
    else if(h->method == DOWNLOAD_SEND) {
        handle_send(arg);
    }
    else if(h->method == DOWNLOAD_RESPONSE) {
        handle_response(arg);
    }
    else {

    }
}


int parse_download_head(void* hptr)
{
    enum {
        pd_start = 0,
        pd_method,
        pd_methodCR,
        pd_methodCRLF,
        pd_arg_start,
        pd_arg,
        pd_argCR,
        pd_argCRLF,
        pd_msglen_start,
        pd_msglen,
        pd_msglenCR,
        pd_msglenCRLF,
        pd_almostdownCR,
        pd_done
    };

    handler_t* h = (handler_t*)hptr;
    char ch;
    char* buf;

    while(h->parse_byte > 0) {
        ch = *(h->parse_ptr);

        switch(h->state) {
            case pd_start:
                if(ch == ' ')
                    break;
                if(ch < 'a' || ch > 'z') {
                    //TODO
                    //error
                }
                h->method_start = h->parse_ptr;
                h->state = pd_method;
                break;

            case pd_method:
                if(ch == ' ') {
                    h->method_end = h->parse_ptr;
                    h->state = pd_methodCR;
                    break;
                }
                if(ch == CR) {
                    h->method_end = h->parse_ptr;
                    h->state = pd_methodCRLF;
                    break;
                }

                if(ch < 'a' || ch > 'z') {
                    //TODO
                    //error
                }
                break;

            case pd_methodCR:
                if(ch == ' ')
                    break;
                if(ch != CR) {
                    //TODO
                    //error
                }
                h->state = pd_methodCRLF;
                break;

            case pd_methodCRLF:
                if(ch != LF) {
                    //TODO
                    //error
                }
                h->state = pd_arg_start;
                break;
            
            case pd_arg_start:
                if(ch == ' ')
                    break;
                if(ch < 'a' || ch > 'z') {
                    //TODO
                    //error
                }
                h->arg_start = h->parse_ptr;
                h->state = pd_arg;
                break;
            
            case pd_arg:
                if(ch == ' ') {
                    h->arg_end = h->parse_ptr;
                    h->state = pd_argCR;
                    break;
                }
                if(ch == CR) {
                    h->arg_end = h->parse_ptr;
                    h->state = pd_argCRLF;
                    break;
                }
                if(ch < 'a' || ch > 'z') {
                    //TODO
                    //error
                }
                break;

            case pd_argCR:
                if(ch == ' ') 
                    break;
                if(ch != CR) {
                    //TODO
                    //error
                }
                h->state = pd_argCRLF;
                break;
            
            case pd_argCRLF:
                if(ch != LF) {
                    //TODO
                    //error
                }

                if(strncmp("send", h->method_start, h->method_end - h->method_start) == 0) {
                    h->method = DOWNLOAD_SEND;
                    h->state = pd_msglen_start;
                    break;
                }
                else
                if(strncmp("response", h->method_start, h->method_end - h->method_start) == 0) {
                    h->method = DOWNLOAD_RESPONSE;
                    h->state = pd_msglen_start;
                    break;
                }
                else
                if(strncmp("close", h->method_start, h->method_end - h->method_start) == 0) {
                    h->method = DOWNLOAD_CLOSE;
                    h->state = pd_almostdownCR;
                    break;
                }
                else
                if(strncmp("connect", h->method_start, h->method_end - h->method_start) == 0) {
                    h->method = DOWNLOAD_CONNECT;
                    h->state = pd_almostdownCR;
                    break;
                }
                //TODO
                //unknown method

            case pd_msglen:
                if(ch == ' ')
                    break;
                if(ch < '0' || ch > '9') {
                    //TODO
                    //error
                }
                h->msglen_start = h->parse_ptr;
                h->state = pd_msglen;
                break;

            case pd_msglen_start:
                if(ch == ' ') {
                    h->msglen_end = h->parse_ptr;
                    h->state = pd_msglenCR;
                    break;
                }
                if(ch == CR) {
                    h->msglen_end = h->parse_ptr;
                    h->state = pd_msglenCRLF;
                    break;
                }
                if(ch >= '0' && ch <= '9')
                    break;
                //TODO
                //error

            case pd_msglenCR:
                if(ch == ' ')
                    break;
                if(ch != CR) {
                    //TODO
                    //error
                }
                h->state = pd_msglenCRLF;
                break;

            case pd_msglenCRLF:
                if(ch != LF) {
                    //TODO
                    //error
                }
                h->state = pd_almostdownCR;
                buf = new char[h->msglen_end - h->msglen_start + 1];
                strncpy(buf, h->msglen_start, h->msglen_end - h->msglen_start);
                h->msglen = atoi(buf);
                delete [] buf;

                break;

            case pd_almostdownCR:
                if(ch != CR) {
                    //TODO
                    //err
                }
                h->state = pd_done;
                break;

            case pd_done:
                h->parse_byte--;
                h->parse_ptr++;
                return PARSE_DOWNLOAD_OK;
        }   /* switch */

        h->parse_ptr++;
        h->parse_byte--;
    }   /* while */
    
    return PARSE_DOWNLOAD_ERROR;
}
