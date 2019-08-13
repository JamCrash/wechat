
#ifndef HANDLE_DOWNLOAD_H
#define HANDLE_DOWNLOAD_H

#include <string>
#include <unordered_map>
#include <unordered_set>

extern std::unordered_map<std::string, int> name2fd;
extern std::unordered_map<std::string, std::unordered_set<std::string>> friend_list;

void download_handler(void* hptr);
void download_send(void* hptr);
void download_connect(void* hptr);
void download_response(void* hptr);
void download_close(void* hptr);

#endif  /* HANDLE_DOWNLOAD_H */