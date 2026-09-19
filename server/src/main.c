#include <inttypes.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <endian.h>

#include "bind_addr.h"
#include "recv_all.h"
#include "send_all.h"

#define PORT "49701"
#define MESSAGE_LEN_SIZE 4
// mr. Chat-gitpy points out that the uint32_t only holds four bytes.
// The MESSAGE_LEN_SIZE could be made larger but that would break this varible type
// It suggests using a _Static_assert like this:
// _Static_assert(sizeof(uint32_t) == MESSAGE_LEN_SIZE, "Filename length field 
// must be 4 bytes");




// Section: main loop
//
int main(void)
{
	struct addrinfo *addr;
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

	int server_fd;
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
	};

	printf("Waiting for connection...\n");

	int client_fd;
	if ((client_fd = accept(server_fd, NULL, NULL)) == -1)
	{
		perror("accept");
		close(server_fd);
		exit(1);
	};

	// We now have the connection. We need to read client message
	// then capture the file requested into some sort of variable
	
	// find the length of the file name
	uint32_t file_name_len; 
	ssize_t mess_len_check = recv_all(client_fd, &file_name_len, MESSAGE_LEN_SIZE, 0);
	if (mess_len_check == -1)
	{
		perror("recv_all did not receive message length");
		exit(1);
	}

	if (mess_len_check != MESSAGE_LEN_SIZE)
	{
		fprintf(stderr, "client disconnected before filename length was received\n");
		exit(1);
	}

	file_name_len = ntohl(file_name_len);

	// use the length of the file name to grab the rest of the file name from buffer
#define MAX_FILE_NAME_LEN 4096
	if (file_name_len == 0 || file_name_len > MAX_FILE_NAME_LEN)
	{
		fprintf(stderr, "Invalid filename length\n");
		exit(1);
	}

	char *file_name_buf = malloc(file_name_len + 1);
	if (file_name_buf == NULL)
	{
		perror("malloc failed");
		exit(1);
	}

	ssize_t file_name_len_check = recv_all(client_fd, file_name_buf, file_name_len, 0);
	if (file_name_len_check == -1)
	{
		perror("recv_all did not receive file name.");
		free(file_name_buf);
		exit(1);
	}

	if ((uint32_t)file_name_len_check != file_name_len)
	{
		fprintf(stderr, "client disconnected before full filename was received\n");
		free(file_name_buf);
		exit(1);
	}
	
	file_name_buf[file_name_len] = '\0';
	printf("File name reads: %s\n", file_name_buf);

	// We want to keep the requested files only coming from allowed directories.
	// For now, we will just prepend this file path 
	#define FS_SHARE_D_PATH "~/Documents/fs_share_d/"

	FILE *requested_file = fopen(file_name_buf, "rb");
	if (requested_file == NULL)
	{
		perror("fopen");
		exit(1);
	}

	struct stat requested_file_info;

	if (fstat(fileno(requested_file), &requested_file_info) == -1)
	{
		perror("fstat");
		exit(1);
	}

	uint64_t requested_file_size = (uint64_t)requested_file_info.st_size;

	printf("File size is: %" PRIu64 " bytes\n", requested_file_size);

	// Using a circular queue we read and send chuncks of the file.
	unsigned char requested_file_buf[4096];
	
	printf("Sending file content...");
	uint64_t requested_file_size_net = htobe64(requested_file_size);

	if (send_all(client_fd, &requested_file_size_net, sizeof requested_file_size, 0) == -1)
	{
		perror("send_all file size failed");
		exit(1);
	}


	size_t bytes_read;
	while ((bytes_read = fread(requested_file_buf, 1, sizeof requested_file_buf, requested_file)) > 0)
	{
		if (send_all(client_fd, requested_file_buf, bytes_read, 0) == -1)
		{
		perror("send_all file content failed");
		exit(1);
		}	
	}

	if (ferror(requested_file))
	{
		perror("fread");
		exit(1);
	}

	printf("Sending complete\n");


	fclose(requested_file);
	free(file_name_buf);
	close(client_fd);
	close(server_fd);

	
}
