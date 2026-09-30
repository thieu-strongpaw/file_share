// server/src/main.c

#include <arpa/inet.h>
#include <endian.h>
#include <inttypes.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "handle_client.h"
#include "fs_core_init.h"


int main(void)
{
	int server_fd = fs_core_init(); // sets up address and starts listening.

	if (server_fd == -1)
	{
		printf("fs_core_init failed.");
	}

	// Would it make sense to have a function that takes care of printing to the terminal?
	// I think that there is an argumenet that having a central engine to drive the the UI
	// could be usefull. 

	printf("Waiting for connection...\n");

	while(1)
	{
	int client_fd;

	client_fd = accept(server_fd, NULL, NULL);
	if (client_fd == -1)
	{
		perror("accept");
		continue;
	};

	int handle_status = handle_client(client_fd);
	if (handle_status == -1)
	{
		fprintf(stderr, "Client request failed\n");
	}

	close(client_fd);
	}

	close(server_fd);

}
