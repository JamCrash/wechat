
#include <fcntl.h>
#include <netinet/in.h>
#include <strings.h>
#include <unordered_map>
#include <unordered_set>
#include <pthread.h>
#include "util.h"
#include "dbg.h"
#include "epoll.h"

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
    event.data.fd = listen_fd;
    epoll.add(listen_fd, &event);

    int n;
    while(1) {
        n = epoll.wait(events, MAXUSER + 1, TIMEOUT);

        for(int i = 0; i < n; ++i) {
            int infd = events[i].data.fd;

            if(infd == listen_fd) {
                //EPOLLONESHOT
                //不应该设置EPOLLET
            }
            else {
                if(fd2name.find(infd) == fd2name.end()) {
                    makethread(sign_in, (void*)&infd);
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
