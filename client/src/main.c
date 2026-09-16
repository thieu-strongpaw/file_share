// file_share client side 
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdint.h>
#include <inttypes.h>
#include <endian.h>

#include "recv_all.h"
#include "send_all.h"

#define PORT "49701"


int main(int argc, char *argv[])
{
	// Client side logic
	struct addrinfo hints, *res, *p;
	int status_getaddr, status_send; 
	int sock_fd;
	
	if (argc != 3) {
		fprintf(stderr, "bad try\n");
		return 1;
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((status_getaddr = getaddrinfo(argv[1], PORT, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status_getaddr));
		exit(1);
	}

	if ((sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
	{
		perror("socket() failed");
		exit(1);
	}

	if ((connect(sock_fd, res->ai_addr, res->ai_addrlen)) == -1)
	{
		perror("connect() failed");
		exit(1);
	}
	
	printf("IP addresses for %s:\n\n", argv[1]);
	printf("Sending data...\n");

	
	char *msg = argv[2];
	uint32_t msg_len = strlen(msg);
	uint32_t net_msg_len = htonl(msg_len);

	// need to construct a packet for my protocal.
	// it is combining two types and IDK how to do that....
	// uint32_t and a char buffer
	if ((status_send = send_all(sock_fd, &net_msg_len, sizeof net_msg_len, 0)) == -1)
	{
		perror("send_all message length failed");
		exit(1);
	}

	if ((status_send = send_all(sock_fd, msg, msg_len, 0) == -1))
	{
		perror("send_all message text failed");
		exit(1);
	}

	printf("File request sent success!\n");

	// Now recieve the length of the requested file
	
	uint64_t file_size_net;

	ssize_t n = recv_all(sock_fd, &file_size_net, sizeof file_size_net, 0);

	if (n == -1) 
	{
		perror("recv_all file size failed");
		exit(1);
	}

	if (n != sizeof file_size_net) 
	{
		fprintf(stderr, "Server disconnected before file size wass received\n");
		exit(1);
	}

	uint64_t file_size = be64toh(file_size_net);

	printf("File size: %" PRIu64 " bytes\n", file_size);

	// now send file content:
	#define FILE_BUF_SIZE 4096

	FILE *output_file = fopen(msg, "wb");

	if (output_file == NULL)
	{
		perror("fopen");
		exit(1);
	}

	char file_buf[FILE_BUF_SIZE];
	int bytes_received = 0;

	while (bytes_received < file_size)
	{
		size_t remaining = file_size - bytes_received;

		size_t chunk_size;
		if (remaining < FILE_BUF_SIZE)
		{
			chunk_size = remaining;
		} else {
			chunk_size = FILE_BUF_SIZE;
		}

		ssize_t n = recv_all(sock_fd, file_buf, chunk_size, 0);

		if (n <= 0) 
		{
			break;
		}

		fwrite(file_buf, 1, n, output_file);

		bytes_received += n;
		}

	fclose(output_file);

	return 0;
};
