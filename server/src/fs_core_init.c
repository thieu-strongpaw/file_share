// server/src/fs_core_init.c

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>

#include "fs_core_init.h"
#include "bind_addr.h"

#define PORT "49701"

int fs_core_init(void)
{
	int server_fd = -1;
	struct addrinfo *addr = NULL;
	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_PASSIVE; 
		
	int status;
	if ((status = getaddrinfo(NULL, PORT, &hints, &addr)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
			exit(1);
	}

	if (bind_addr(addr, &server_fd) == -1) 
	{
		perror("bind failed");
		freeaddrinfo(addr);
		exit(1); // could not bind to the server address. Crash program.
	}

	freeaddrinfo(addr);

	if (listen(server_fd, 10) == -1)
	{
		perror("listen failed");
		close(server_fd);
		exit(1);
	}

	return server_fd;
}
