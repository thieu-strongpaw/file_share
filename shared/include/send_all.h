#ifndef SEND_ALL_H
#define SEND_ALL_H

#include <stdlib.h>

ssize_t send_all(int fd, void *buf, size_t buf_len, int flag);

#endif
