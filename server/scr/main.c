// file_share server component to share files between hosts

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 1701
#define MESSAGE_LEN_SIZE 4

int main(void)
{
	int server_fd, client_fd;
	struct sockaddr_in addr;
	char message_size[MESSAGE_LEN_SIZE];
	int receiver = 0;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT);

	bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
	listen(server_fd, 1);

	printf("Waiting for connection...\n");

	client_fd = accept(server_fd, NULL, NULL);

	// We now have the connection. We need to read client message
	// then capture the file requested into some sort of variable
	
	// find the length of the file name
	while(receiver < MESSAGE_LEN_SIZE)
	{
		int n = recv(client_fd, message_size, sizeof(message_size), 0);
		if (n <= 0) break;
		receiver += n;
	}

	// use the length of the file name to grab the rest of the file name from buffer
	char file_name_buf[(int)&message_size];
	while(receiver < message_size)
	{
		int n = recv(client_fd, file_name_buf, sizeof(file_name_buf), 0);
		if(n <= 0) break;
		receiver += n;

	}
	printf("file name: s%", &file_name_buf);
	

	return 0;
}
