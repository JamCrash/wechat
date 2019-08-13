
#include <string.h>
#include "handle_download.h"
#include "parse_upload.h"
#include "handler.h"
#include "util.h"

//以下函数暂时假设参数不会出错
void download_handler(void* hptr)
{
    handler_t* h = (handler_t*)hptr;

    switch(h->method) {
        case UPLOAD_LIST:
        case UPLOAD_SEND:
            //TODO
            //method send
        case UPLOAD_CLOSE:
            if(strncmp(h->arg_start, "all", 3) == 0) {
                //method close
            }else {

            }
            break;
        case UPLOAD_CONNECT:
            //TODO
            //method connect
        case UPLOAD_RESPONSE:
            //TODO
            //method response
        default:
            //TODO
            //error
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
        cur_pos += sprintf(cur_pos, "send\r\nserver\r\n");

        std::string tmpstr;
        for(auto& item: name2fd) {
            tmpstr += item.first;
            tmpstr += '\n';
        }
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
        cur_pos += sprintf(cur_pos, "send\r\n%s\r\n", from_whom.c_str());
        cur_pos += sprintf(cur_pos, "%d\r\n\r\n", h->msglen);
    }
}

void download_close(void* hptr)
{

}

void download_connect(void* hptr)
{

}

void download_response(void* hptr)
{

}
