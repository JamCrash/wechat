
#ifndef PARSE_DOWNLOAD_H
#define PARSE_DOWNLOAD_H

#define PARSE_DOWNLOAD_ERROR    0x0100
#define PARSE_DOWNLOAD_OK       0x0200

#define CR  '\r'
#define LF  '\n'

#define DOWNLOAD_SEND       0x1010
#define DOWNLOAD_CONNECT    0x2020
#define DOWNLOAD_RESPONSE   0x3030
#define DOWNLOAD_CLOSE      0x4040

void* read_info(void* arg);
int parse_download_head(void* hptr);
void print_msg(void* arg);

#endif  /* PARSE_DOWNLOAD_H */
