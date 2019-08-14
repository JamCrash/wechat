
#define _GUN_SOURCE

#include <fcntl.h>
#include <netinet/in.h>
#include <strings.h>
#include <unordered_map>
#include <unordered_set>
#include <pthread.h>
#include "util.h"
#include "dbg.h"
#include "epoll.h"
#include "handler.h"

#define MAXUSER 8
#define TIMEOUT 400

struct epoll_event events[MAXUSER + 1];

typedef struct {
    char will_be_write[BUFSIZE];
    int total_byte = 0;
}data;

std::unordered_map<int, std::string> fd2name;
std::unordered_map<std::string, int> name2fd;
std::unordered_map<int, data*> fd2data;
std::unordered_map<std::string, std::string> pwd_list;
std::unordered_map<std::string, std::unordered_set<std::string>> friend_list;

extern void* do_request(void* fdptr);

int main()
{
    int listen_fd;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    int flag = fcntl(listen_fd, F_GETFD, 0);
    flag |= O_NONBLOCK;
    fcntl(listen_fd, F_SETFD, flag);
 
    struct sockaddr_in listen_sock;
    bzero(&listen_sock, sizeof(sockaddr_in));
    listen_sock.sin_addr.s_addr = htonl(INADDR_ANY);
    listen_sock.sin_family = AF_INET;
    listen_sock.sin_port = htons((unsigned short)4000);

    bind(listen_fd, (sockaddr*)&listen_sock, sizeof(listen_sock));

    listen(listen_fd, MAXUSER + 1);

    epoll_t epoll(MAXUSER + 1);
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.events = EPOLLIN | EPOLLET;
    event.data.ptr = new handler_t(listen_fd);
    epoll.add(listen_fd, &event);

    int n;
    while(1) {
        n = epoll.wait(events, MAXUSER + 1, TIMEOUT);

        for(int i = 0; i < n; ++i) {
            handler_t* h = (handler_t*)events[i].data.ptr;
            int fd = h->fd;

            int infd;
            if(fd == listen_fd) {
                while(1) {
                    infd = accept(listen_fd, NULL, NULL);
                    if(infd < 0) {
                        if((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                            break;
                        }
                        else {
                            log_err("accept error");
                            break;
                        }
                    }

                    bzero(&event, sizeof(event));
                    event.events = EPOLLIN | EPOLLOUT | EPOLLONESHOT | EPOLLET | EPOLLRDHUP;
                    event.data.ptr = (void*)new handler_t(infd);
                    epoll.add(infd, &event);
                }
            }
            else {
                if(fd2name.find(infd) == fd2name.end()) {
                    if(events[i].events & EPOLLRDHUP) {
                        delete events[i].data.ptr;
                        epoll.del(infd, NULL);
                    }
                    else {
                        //sign in
                        makethread(sign_in, (void*)&infd, int);
                    }
                }
                else {
                    
                    if(events[i].events & EPOLLERR) {
                        
                    }
                    else if(events[i].events & EPOLLRDHUP) {

                    }
                    else if(events[i].events & EPOLLIN) {
                        //用户已经存在并登录
                        //创建一个线程来处理用户请求
                        //主要是请求连接或者关闭链接
                        
                    }
                    else if(events[i].events & EPOLLOUT) {
                        if(strlen(fd2data[infd]->will_be_write) > 0) {

                        }
                    }
                    else {

                    }
                }
            }
        }
    }
}
