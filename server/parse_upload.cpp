
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "parse_upload.h"
#include "handler.h"
#include "handle_download.h"


#define SMALLSIZE   8

//未完成
void* parse_upload(void* hptr)
{
    handler_t* h = (handler_t*)hptr;
    int fd = h->fd;

    int n;
    int parse_state;
    int head_remain_size = HEADLEN;
    int msg_remain_size;

    while(1) {
        n = read(fd, h->cur_pos, head_remain_size);
        if(n == 0) 
            break;
        if(n < 0) {
            if((errno == EAGAIN) || (errno == EWOULDBLOCK))
                break;
            else {
                goto err;
            }
        }

        h->cur_pos += n;
        h->parse_byte += n;
        head_remain_size -= n;
        if(head_remain_size > 0 && ((parse_state = parse_upload_head(hptr)) == PARSE_UPLOAD_AGAIN)) {
            continue;
        }
        else if(head_remain_size != 0 || parse_state != PARSE_UPLOAD_OK) {
            goto err;
        }

        //parse header finish
        msg_remain_size = h->msglen;
        while(1) {
            n = read(fd, h->cur_pos, msg_remain_size);
            if(n == 0)
                break;
            if(n < 0) {
                if((errno == EAGAIN) || (errno == EWOULDBLOCK))
                    break;
                else {
                    goto err;
                }
            }

            msg_remain_size -= n;
            h->cur_pos += n ;
            if(msg_remain_size == 0) 
                break;
        }

        if(msg_remain_size == 0) {
            download_handler(hptr);
            h->remake();
            break;
        }
    }
    struct epoll_event event;
    memset(&event, '\0', sizeof(event));
    event.events = EPOLLIN | EPOLLOUT | EPOLLONESHOT | EPOLLRDHUP;
    event.data.ptr = hptr;
    h->epoll_ptr->mod(h->fd, &event);
    return NULL;
err:
    //TODO
    //向所有连接的好友发消息
    close(fd);
    return NULL;
}

int parse_upload_head(void* hptr)
{
    enum {
        pu_done = -1,
        pu_start,
        pu_method,
        pu_methodCR,
        pu_methodCRLF,
        pu_arg_start,
        pu_arg,
        pu_argCR,
        pu_argCRLF,
        pu_msglen_start,
        pu_msglen,
        pu_msglenCR,
        pu_msglenCRLF,
        pu_almostdoneCR,
        pu_almostdoneCRLF
    }state;
    
    handler_t* h = (handler_t*)hptr;
    if(h->state == pu_done)
        return PARSE_UPLOAD_OK; //直接处理该情况，防止引用非法内存地址

    char ch = *(h->parse_ptr);
    
    while(h->parse_byte > 0) {
        switch(h->state) {
            case pu_start:
                if(ch == ' ')
                    break;
                else if(ch < 'a' || ch > 'z') {
                    //TODO
                    //error
                }

                h->method_start = h->parse_ptr;
                h->state = pu_method;
                break;

            case pu_method:
                if(ch < 'a' || ch > 'z') {
                    //TODO
                    //error
                }
                if(ch == ' ') {
                    h->method_end = h->parse_ptr;
                    h->state = pu_methodCR;
                    break;
                }
                if(ch == CR) {
                    h->method_end = h->parse_ptr;
                    h->state = pu_methodCRLF;
                    break;
                }
                if(!(ch >= 'a' && ch <= 'z')) {
                    //TODO
                    //error
                }
                break;

            case pu_methodCR:
                if(ch == ' ') 
                    break;
                if(ch != CR) {
                    //TODO
                    //error
                }
                h->state = pu_methodCRLF;
                break;

            case pu_methodCRLF:
                if(ch != LF) {
                    //TODO
                    //error
                }
                h->state = pu_arg_start;
                break;

            case pu_arg_start:
                if( !((ch >= 'a' && ch <= 'z')
                    || (ch >= 'A' && ch <= 'Z')
                    || (ch >= '0' && ch <= '9')
                    || (ch == '_')) ) {
                        //TODO
                        //error
                    }
                h->arg_start = h->parse_ptr;
                h->state = pu_arg;
                break;
            
            case pu_arg:
                if(ch == ' ') {
                    h->arg_end = h->parse_ptr;
                    h->state = pu_argCR;
                    break;
                }
                if(ch == CR) {
                    h->arg_end = h->parse_ptr;
                    h->state = pu_argCRLF;
                    break;
                }
                if( !((ch >= 'a' && ch <= 'z')
                    || (ch >= 'A' && ch <= 'Z')
                    || (ch >= '0' && ch <= '9')
                    || (ch == '_')) ) {
                        //TODO
                        //error
                    }
                break;

            case pu_argCR:
                if(ch == ' ') 
                    break;
                if(ch != CR) {
                    //TODO
                    //error
                }
                h->state = pu_argCRLF;
                break;
            
            case pu_argCRLF:
                if(ch != LF) {
                    //TODO
                    //error
                }

                if(strncmp(h->method_start, "send", 4) == 0) {
                    h->method = UPLOAD_SEND;
                    h->state = pu_msglen_start;
                    break;
                }
                if(strncmp(h->method_start, "response", 8) == 0) {
                    h->method = UPLOAD_RESPONSE;
                    h->state = pu_msglen_start;
                    break;
                }
                if(strncmp(h->method_start, "list", 4) == 0) {
                    h->method = UPLOAD_LIST;
                    h->state = pu_almostdoneCR;
                    break;
                }
                if(strncmp(h->method_start, "connect", 7) == 0) {
                    h->method = UPLOAD_CONNECT;
                    h->state = pu_almostdoneCR;
                    break;
                }
                if(strncmp(h->method_start, "close", 5) == 0) {
                    h->method = UPLOAD_CLOSE;
                    h->state = pu_almostdoneCR;
                    break;
                }
                //TODO  error
                //unknown method
                //break;

            case pu_msglen_start:
                if(!(ch >= '0' && ch <= '9')) {
                    //TODO
                    //error
                }
                h->msglen_start = h->parse_ptr;
                h->state = pu_msglen;
                break;

            case pu_msglen:
                if(ch == ' ') {
                    h->msglen_end = h->parse_ptr;
                    char tmpbuf[SMALLSIZE];
                    strncpy(tmpbuf, h->msglen_start, h->msglen_end - h->msglen_start);
                    tmpbuf[h->msglen_end - h->msglen_start] = '\0';
                    h->msglen = atoi(tmpbuf);
                    h->state = pu_msglenCR;
                    break;
                }
                if(ch == CR) {
                    h->msglen_end = h->parse_ptr;
                    char tmpbuf[SMALLSIZE];
                    strncpy(tmpbuf, h->msglen_start, h->msglen_end - h->msglen_start);
                    tmpbuf[h->msglen_end - h->msglen_start] = '\0';
                    h->msglen = atoi(tmpbuf);
                    h->state = pu_msglenCRLF;
                    break;
                }
                if(!(ch >= '0' && ch <= '9')) {
                    //TODO
                    //error
                }
                break;

            case pu_msglenCR:
                if(ch == ' ')
                    break;
                if(ch != CR) {
                    //TODO
                    //error
                }
                h->state = pu_msglenCRLF;
                break;
            
            case pu_msglenCRLF:
                if(ch != LF) {
                    //TODO
                }
                h->state = pu_almostdoneCR;
                break;
            
            case pu_almostdoneCR:
                if(ch != CR) {
                    //TODO
                }
                h->state = pu_almostdoneCRLF;
                break;

            case pu_almostdoneCRLF:
                if(ch != LF) {
                    //TODO
                }
                h->state = pu_done;
                h->parse_byte--;
                h->parse_ptr++;
                return PARSE_UPLOAD_OK;
        }   /* switch */

        h->parse_byte--;
        h->parse_ptr++;
    }   /* while */

    return PARSE_UPLOAD_AGAIN;
}
