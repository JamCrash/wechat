
#include <string.h>
#include "handle_download.h"
#include "parse_upload.h"
#include "handler.h"
#include "util.h"
#include "dbg.h"

//以下函数暂时假设参数不会出错
//假设缓存足够大
//加锁
void download_handler(void* hptr)
{
    handler_t* h = (handler_t*)hptr;

    switch(h->method) {
        case UPLOAD_LIST:
        case UPLOAD_SEND:
            download_send(hptr);
            break;

        case UPLOAD_CLOSE:
            download_close(hptr);
            break;

        case UPLOAD_CONNECT:
            download_connect(hptr);
            break;

        case UPLOAD_RESPONSE:
            download_response(hptr);
            break;

        default:
            log_err("unknown method");
            //TODO
            break;
    }   /* switch */
}

void download_send(void* hptr)
{ 
    handler_t* h = (handler_t*)hptr;
    char* cur_pos;
 
    if(h->method == UPLOAD_LIST) {
        //暂时不做参数检查
        //参数必定为all
        data* user_data_bufer = fd2data[h->fd];
        cur_pos = user_data_bufer->will_be_write + user_data_bufer->total_byte;

        std::string tmpstr;
        for(auto& item: name2fd) {
            tmpstr += item.first;
            tmpstr += '\n';
        }

        //先写入前导空格
        size_t space_num = HEADLEN 
                            - strlen("send\r\nserver\r\n") 
                            - std::to_string((int)tmpstr.size()).size()
                            - strlen("\r\n\r\n");
        memset(cur_pos, ' ', space_num);
        cur_pos += space_num;

        cur_pos += sprintf(cur_pos, "send\r\nserver\r\n");
        cur_pos += sprintf(cur_pos, "%d\r\n\r\n", tmpstr.size());
        cur_pos += sprintf(cur_pos, "%s", tmpstr.c_str());

        user_data_bufer->total_byte = (cur_pos - user_data_bufer->will_be_write);
    }
    else {
        //UPLOAD_SEND
        std::string to_whom(h->arg_start, h->arg_end);
        std::string from_whom = fd2name[h->fd];

        data* to_whom_bufer = fd2data[name2fd[to_whom]];
        cur_pos = to_whom_bufer->will_be_write + to_whom_bufer->total_byte;

        //写入前导空格
        size_t space_num = HEADLEN
                            - strlen("send\r\n")
                            - (from_whom + "\r\n").size()
                            - std::to_string(h->msglen).size()
                            - strlen("\r\n\r\n");
        memset(cur_pos, ' ', space_num);
        cur_pos += space_num;

        cur_pos += sprintf(cur_pos, "send\r\n%s\r\n", from_whom.c_str());
        cur_pos += sprintf(cur_pos, "%d\r\n\r\n", h->msglen);
        strncpy(cur_pos, h->parse_ptr, h->msglen);
        cur_pos += h->msglen;

        to_whom_bufer->total_byte = (cur_pos - to_whom_bufer->will_be_write);
    }
}

void download_close(void* hptr)
{
    handler_t* h = (handler_t*)hptr;
    std::string arg(h->arg_start, h->arg_end);

    if(arg == std::string("all")) {
        std::string from_whom(fd2name[h->fd]);
        
        for(auto& to_whom: friend_list[from_whom]) {
            data* to_whom_bufer = fd2data[name2fd[to_whom]];
            char* cur_pos = to_whom_bufer->will_be_write + to_whom_bufer->total_byte;

            size_t space_num = HEADLEN
                                - strlen("close\r\n")
                                - from_whom.size()
                                - strlen("\r\n\r\n");
            memset(cur_pos, ' ', space_num);
            cur_pos += space_num;

            cur_pos += sprintf(cur_pos, "close\r\n");
            cur_pos += sprintf(cur_pos, "%s\r\n\r\n", from_whom.c_str());

            to_whom_bufer->total_byte = (cur_pos - to_whom_bufer->will_be_write);

            friend_list[to_whom].erase(from_whom);
        }

        friend_list.erase(from_whom);
    }
    else {
        std::string& to_whom = arg;
        std::string  from_whom(fd2name[h->fd]);

        data* to_whom_bufer = fd2data[name2fd[to_whom]];
        char* cur_pos = to_whom_bufer->will_be_write + to_whom_bufer->total_byte;

        size_t space_num = HEADLEN
                            - strlen("close\r\n")
                            - from_whom.size()
                            - strlen("\r\n\r\n");
        memset(cur_pos, ' ', space_num);
        cur_pos += space_num;

        cur_pos += sprintf(cur_pos, "close\r\n");
        cur_pos += sprintf(cur_pos, "%s\r\n\r\n", from_whom.c_str());

        to_whom_bufer->total_byte = (cur_pos - to_whom_bufer->will_be_write);

        friend_list[to_whom].erase(from_whom);
        friend_list[from_whom].erase(to_whom);
    }
}

void download_connect(void* hptr)
{
    handler_t* h = (handler_t*)hptr;
    std::string from_whom(fd2name[h->fd]);
    std::string to_whom(h->arg_start, h->arg_end);

    data* to_whom_bufer = fd2data[name2fd[to_whom]];
    char* cur_pos = to_whom_bufer->will_be_write + to_whom_bufer->total_byte;

    size_t space_num = HEADLEN
                        - strlen("connect\r\n")
                        - from_whom.size()
                        - strlen("\r\n\r\n");
    memset(cur_pos, ' ', space_num);
    cur_pos += space_num;

    cur_pos += sprintf(cur_pos, "connect\r\n");
    cur_pos += sprintf(cur_pos, "%s\r\n\r\n", from_whom.c_str());

    to_whom_bufer->total_byte = (cur_pos - to_whom_bufer->will_be_write);
}

void download_response(void* hptr)
{
    handler_t* h = (handler_t*)hptr;
    std::string from_whom(fd2name[h->fd]);
    std::string to_whom(h->arg_start, h->arg_end);

    data* to_whom_bufer = fd2data[name2fd[to_whom]];
    char* cur_pos = to_whom_bufer->will_be_write + to_whom_bufer->total_byte;

    size_t space_num = HEADLEN
                        - strlen("response\r\n")
                        - (from_whom + "\r\n").size()
                        - std::to_string(h->msglen).size()
                        - strlen("\r\n\r\n");
    memset(cur_pos, ' ', space_num);
    cur_pos += space_num;

    cur_pos += sprintf(cur_pos, "response\r\n");
    cur_pos += sprintf(cur_pos, "%s\r\n", from_whom.c_str());
    cur_pos += sprintf(cur_pos, "%d\r\n\r\n", h->msglen);
    strncpy(cur_pos, h->parse_ptr, h->msglen);
    cur_pos += h->msglen;

    to_whom_bufer->total_byte = (cur_pos - to_whom_bufer->will_be_write);
}
