// file_share client side 
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT "49701"


ssize_t send_all(int fd, void *buf, size_t buf_len, int flag)
{
	int sent = 0;
	char *ptr = buf;

	while(sent < buf_len)
	{
		ssize_t n = send(fd, ptr + sent, buf_len - sent, flag);
		if (n <  0) return -1;
		if (n == 0) break; // peer closed connection
		sent += n;
	}

	return sent;
}

int main(int argc, char *argv[])
{
	// Client side logic
	struct addrinfo hints, *res, *p;
	int status_getaddr, status_send; 
	int sock_fd;
	char ipstr[INET6_ADDRSTRLEN];
	
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
	if ((status_send = send_all(sock_fd, &net_msg_len, sizeof net_msg_len, 0) == -1))
	{
		perror("send_all message length failed");
		exit(1);
	}

	if ((status_send = send_all(sock_fd, msg, msg_len, 0) == -1))
	{
		perror("send_all message text failed");
		exit(1);
	}

	printf("Message sent success!\n");

	return 0;
};
