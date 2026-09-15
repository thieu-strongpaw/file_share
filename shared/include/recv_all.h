#ifndef RECV_ALL_H
#define RECV_ALL_H

#include <stdio.h>

ssize_t recv_all(int fd, void *buf, size_t buf_len, int flag);

#endif
