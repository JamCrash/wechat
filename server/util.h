
#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <unordered_map>
#include <unordered_set>

#define BUFSIZE 512

#define makethread(fn, rarg_ptr, TYPE)  \
({  \
    TYPE tmparg = *(TYPE*)rarg_ptr; \
    int err;    \
    pthread_t tid;  \
    pthread_attr_t  attr;   \
    err = pthread_attr_init(&attr); \
    if(err == 0) {  \
        err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);  \
        if(err == 0) {  \
            err == pthread_create(&tid, &attr, fn, &tmparg);    \
        }   \
        pthread_attr_destroy(&attr);    \
    }   \
    err;    \
})

typedef struct {
    char will_be_write[BUFSIZE];
    int total_byte = 0;
}data;

extern std::unordered_map<int, std::string> fd2name;
extern std::unordered_map<std::string, int> name2fd;
extern std::unordered_map<int, data*> fd2data;
extern std::unordered_map<std::string, std::string> pwd_list;
extern std::unordered_map<std::string, std::unordered_set<std::string>> friend_list;

void* sign_in(void* fdptr);

#endif  /* UTIL_H */
