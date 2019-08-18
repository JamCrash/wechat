
#ifndef HANDLE_UPLOAD_H
#define HANDLE_UPLOAD_H

#define BUFSIZE 512

#define UPLOAD_LIST     0x1122
#define UPLOAD_SEND     0x2233
#define UPLOAD_CONNECT  0x3344
#define UPLOAD_RESPONSE 0x4455
#define UPLOAD_CLOSE    0x5566
#define UPLOAD_UNKNOWN  0xffff

struct parser {
    int     method;
    char*   arg_start;
};

void send_msg(int fd, void* arg);
void parse_method(char* bufer, size_t size, struct parser* p);

void upload_send(char* buf, size_t size, parser* p);
void upload_response(char* buf, size_t size, parser* p);
void upload_close(char* buf, size_t size, parser* p);
void upload_list(char* buf, size_t size, parser* p);
void upload_connect(char* buf, size_t size, parser* p);

#endif  /* HANDLE_UPLOAD_H */
