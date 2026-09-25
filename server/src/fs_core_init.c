#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>

#include "bind_addr.h"

#define PORT "49701"

int fs_core_init(struct addrinfo *addr, int server_fd)
{
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

	if (listen(server_fd, 1) == -1)
	{
		perror("listen failed");
		close(server_fd);
		freeaddrinfo(addr);
		exit(1);
	}

	return 1;
}
