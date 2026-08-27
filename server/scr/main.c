// file_share server component to share files between hosts
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>

#define PORT "49701"
#define MESSAGE_LEN_SIZE 4
// mr. Chat-gitpy points out that the uint32_t only holds four bytes.
// The MESSAGE_LEN_SIZE could be made larger but that would break this varible type
// It suggests using a _Static_assert like this:
// _Static_assert(sizeof(uint32_t) = FILE_NAME_LEN_SIZE, "Filename length field 
// must be 4 bytes");

// SECTION: function declarations

// recv_all() wraps recv() to insure that all bytes sent are actually recieved
// fd is the remote connections file discriptor.
// buf is a pointer to a buffer to receive the data.
ssize_t recv_all(int fd, void *buf, size_t buf_len, int flag)
{
	int received = 0;
	char *ptr = buf;

	while(received < buf_len)
	{
		ssize_t n = recv(fd, ptr + received, buf_len - received, flag);
		if (n <  0) return -1;
		if (n == 0) break; // peer closed connection
		received += n;
	}

	return received;
}

/* bind_addr() wraps the bind() function. It will walk the linked
 * returned by getaddrinfo() and returns the file discriptor of the 
 * first node to bind successfully.
 * returns -1 on failure
 * returns 1 on success
*/ 
int bind_addr(struct addrinfo* addr_list, int *sock_fd)
{
	struct addrinfo *p;
	*sock_fd = -1;

	for (p = addr_list; p != NULL; p = p->ai_next)
	{
		*sock_fd = socket(
				p->ai_family,
				p->ai_socktype,
				p->ai_protocol
			);

		if (*sock_fd == -1)
			continue;

		int yes = 1;

		if (setsockopt(*sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
		{
			perror("setsockopt");
			close(*sock_fd);
			*sock_fd = -1;
			continue;
		}
		if (bind(*sock_fd, p->ai_addr, p->ai_addrlen) == -1) {
			close(*sock_fd);
			*sock_fd = -1;
			continue;
		}
		return 1;
	}
	return -1;
}

// Section: main loop
//
int main(void)
{
	int client_fd = -1;
	int server_fd = -1;
	struct addrinfo *addr;
	struct addrinfo hints;
	uint32_t file_name_len; 

	memset(&hints, 0, sizeof hints);

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_PASSIVE; 

	int status = getaddrinfo(
			NULL,
			PORT,
			&hints,
			&addr
			);

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
		exit(1);
	};

	printf("Waiting for connection...\n");

	if ((client_fd = accept(server_fd, NULL, NULL)) == -1)
	{
		perror("accept");
		close(server_fd);
		exit(1);
	};

	// We now have the connection. We need to read client message
	// then capture the file requested into some sort of variable
	
	// find the length of the file name
	ssize_t mess_len_check = recv_all(client_fd, &file_name_len, MESSAGE_LEN_SIZE, 0);
	if (mess_len_check == -1)
	{
		perror("recv_all did not receive message length");
		exit(1);
	}

	printf("received length: %zd bytes\n", mess_len_check);

	if (mess_len_check != MESSAGE_LEN_SIZE)
	{
		fprintf(stderr, "client disconnected before filename length was received\n");
		exit(1);
	}

	file_name_len = ntohl(file_name_len);

	// use the length of the file name to grab the rest of the file name from buffer
	char *file_name_buf = malloc(file_name_len + 1);
	if (file_name_buf == NULL)
	{
		perror("malloc failed");
		exit(1);
	}

	ssize_t mess_check = recv_all(client_fd, file_name_buf, file_name_len, 0);
	if (mess_check > 0)
	{
		file_name_buf[file_name_len] = '\0';
		printf("Message reads: %s\n", file_name_buf);
	}


	return 0;
}
