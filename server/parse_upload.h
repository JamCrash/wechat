
#ifndef PARSE_UPLOAD_H
#define PARSE_UPLOAD_H

#define PARSE_UPLOAD_AGAIN  1
#define PARSE_UPLOAD_OK     0

#define UPLOAD_LIST         0x0010
#define UPLOAD_RESPONSE     0x0020
#define UPLOAD_CONNECT      0x0030
#define UPLOAD_SEND         0x0040
#define UPLOAD_CLOSE        0x0050

#define CR  '\r'
#define LF  '\n'

#define HEADLEN     40

void* parse_upload(void* hptr);
int   parse_upload_head(void* h);

#endif  /* PARSE_UPLOAD_H */