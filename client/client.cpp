
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <unordered_set>
#include <string>
#include "dbg.h"
#include "util.h"
#include "parse_download.h"
#include "handler.h"
#include "handle_upload.h"

#define SMALLBUFSIZE    64
#define INPUTBUFSIZE    1024

std::unordered_set<std::string> friend_list;

int main()
{
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons((unsigned short)4000);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(make_socket_nonblock(server_fd) < 0) {
        log_err("make client fd nonblock failed");
        close(server_fd);
        return 0;
    }

    if(connect(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        log_err("connection failed");
        close(server_fd);
        return 0;
    }

    if(check_in(server_fd) != 0) {
        close(server_fd);
        return 0;
    }

    handler_t* handler = new handler_t(server_fd);

    char input_bufer[INPUTBUFSIZE];
    int n;
    pthread_t tid;
    if(makethread(&tid, read_info, (void*)handler) != 0) {
        log_err("make read thread failed\n");
        return 0;
    }

    //主线程 也即写线程
    send_msg(server_fd, (void*)&tid);
}
