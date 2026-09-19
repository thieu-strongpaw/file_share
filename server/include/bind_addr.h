#ifndef BIND_ADDR_H
#define BIND_ADDR_H

#include <netdb.h>

int bind_addr(struct addrinfo* addr_list, int *sock_fd);

#endif
