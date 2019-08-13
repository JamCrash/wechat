
#include <stdio.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include "util.h"

void* sign_in(void* fdptr)
{
    int fd = *(int*)fdptr;
    FILE* fp;
    char name_bufer[BUFSIZE];
    char pwd_bufer[BUFSIZE];
    char bufer[BUFSIZE];
    std::string name, pwd;

    fp = fdopen(fd, "r+");
    if(fp == NULL) 
        return NULL;
    

    while(1) {
        //TODO
        //how to exit??
        fprintf(fp, "1: sign in\n2: sign up\n3: exit\n");
        fgets(bufer, BUFSIZE, fp);

        bufer[strlen(bufer) - 1] = '\0';
        if(strncmp(bufer, "1", 1) == 0) {
            //sign in
            fprintf(fp, "Please input your user name: \n");
            fgets(name_bufer, BUFSIZE, fp);

            name_bufer[strlen(name_bufer) - 1] = '\0';
            name = name_bufer;

            auto user_it = pwd_list.find(name);
            if(user_it == pwd_list.end()) {
                fprintf(fp, "User doesn't exit\n");
                continue;
            }
            else {
                fprintf(fp, "Please input password: \n");
                fgets(pwd_bufer, BUFSIZE, fp);

                pwd_bufer[strlen(pwd_bufer) - 1] = '\0';
                pwd = pwd_bufer;
                if(pwd == user_it->second) {
                    //password correct
                    fd2name[fd] = name;
                    name2fd[name] = fd;
                    fd2data[fd] = new data;
                    fprintf(fp, "Login successfully\n");
                    break;
                }
                else {
                    //password error
                    fprintf(fp, "Password incorrect\n");
                    continue;
                }
            }
        }   /* sign in */
        else if(strncmp(bufer, "2", 1) == 0) {
            //sign up
            fprintf(fp, "Sign up: please input yout user name: \n");
            fgets(name_bufer, BUFSIZE, fp);
            name_bufer[strlen(name_bufer) - 1] = '\0';
            name = name_bufer;

            fprintf(fp, "Sign up: please input your password: \n");
            fgets(pwd_bufer, BUFSIZE, fp);
            pwd_bufer[strlen(pwd_bufer) - 1] = '\0';
            pwd = pwd_bufer;

            fprintf(fp, "Register successfully\n");
            
            continue;
        }
        else if(strncmp(bufer, "exit", 4) == 0) {
            //exit
            //TODO
            //should server say something??
            close(fd);
        }
        else {
            //inllegal input
            fprintf(fp, "Illegal input\n");
            continue;
        }
    }   /* while */

    /* login successfully */
}

int makethread(void* (*fn)(void*), void* arg)
{
    int err;
    pthread_t pid;
    pthread_attr_t attr;

    err = pthread_attr_init(&attr);
    if(err != 0) {
        return err;
    }
    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if(err == 0) {
        err = pthread_create(&pid, &attr, fn, arg);
    }
    pthread_attr_destroy(&attr);
    return err;
}
