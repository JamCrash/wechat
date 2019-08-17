
#include <stdio.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include "util.h"
#include "dbg.h"

//TODO
//check n
void* sign_in(void* fdptr)
{
    int fd = *(int*)fdptr;
    int n;
    char name_bufer[BUFSIZE];
    char pwd_bufer[BUFSIZE];
    char bufer[BUFSIZE];
    std::string name, pwd;

    while(1) {
        //TODO
        //how to exit??
        write(fd, "1: sign in\n2: sign up\n3: exit\n", strlen("1: sign in\n2: sign up\n3: exit\n"));
        n = read(fd, bufer, BUFSIZE);
        
        bufer[n - 1] = '\0';
        if(strncmp(bufer, "1", 1) == 0) {
            //sign in
            write(fd, "Please input your user name: \n", strlen("Please input your user name: \n"));
            n = read(fd, name_bufer, BUFSIZE);

            name_bufer[n - 1] = '\0';
            name = name_bufer;

            auto user_it = pwd_list.find(name);
            if(user_it == pwd_list.end()) {
                write(fd, "User doesn't exist\n", strlen("User doesn't exist\n"));
                continue;
            }
            else {
                write(fd, "Please input password: \n", strlen("Please input password: \n"));
                n = read(fd, pwd_bufer, BUFSIZE);

                pwd_bufer[n - 1] = '\0';
                pwd = pwd_bufer;
                if(pwd == user_it->second) {
                    //password correct
                    fd2name[fd] = name;
                    name2fd[name] = fd;
                    fd2data[fd] = new data;
                    write(fd, "Login successfully\n", strlen("Login successfully\n"));
                    break;
                }
                else {
                    //password error
                    write(fd, "Password incorrect\n", strlen("Password incorrect\n"));
                    continue;
                }
            }
        }   /* sign in */
        else if(strncmp(bufer, "2", 1) == 0) {
            //sign up
            write(fd, "Sign up: please input yout user name: \n", strlen("Sign up: please input yout user name: \n"));
            n = read(fd, name_bufer, BUFSIZE);
            name_bufer[n - 1] = '\0';
            name = name_bufer;

            write(fd, "Sign up: please input your password: \n", strlen("Sign up: please input your password: \n"));
            n = read(fd, pwd_bufer, BUFSIZE);
            pwd_bufer[n - 1] = '\0';
            pwd = pwd_bufer;

            pwd_list[name] = pwd;

            write(fd, "Register successfully\n", strlen("Register successfully\n"));
            
            break;
        }   /* sign up */
        else if((strncmp(bufer, "exit", 4) == 0) || strncmp(bufer, "3", 1) == 0) {
            //exit
            //should server say something??
            printf("connection fd=%d exit\n", fd);
            write(fd, "exit\n", strlen("exit\n"));
            close(fd);
            break;
        }
        else {
            //inllegal input
            write(fd, "Illegal input\n", strlen("Illegal input\n"));
            continue;
        }
    }   /* while */

    return NULL;
}
