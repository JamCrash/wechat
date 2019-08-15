
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include "dbg.h"
#include "util.h"

#define SMALLBUFSIZE 64

int main()
{
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons((unsigned short)4000);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(make_socket_nonblock(client_fd) < 0) {
        log_err("make client fd nonblock failed");
        close(client_fd);
        return 0;
    }
    
    if(connect(client_fd, (sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        log_err("connection failed");
        close(client_fd);
        return 0;
    }


}