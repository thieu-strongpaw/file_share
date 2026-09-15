
#include <stdlib.h>
#include <sys/socket.h>

// recv_all() wraps recv() to insure that all bytes sent are actually recieved
// fd is the remote connections file discriptor.
// buf is a pointer to a buffer to receive the data.
ssize_t recv_all(int fd, void *buf, size_t buf_len, int flag)
{
	size_t received = 0;
	char *ptr = buf;

	while(received < buf_len)
	{
		ssize_t n = recv(fd, ptr + received, buf_len - received, flag);
		if (n <  0) return -1; // recv error
		if (n == 0) break; // peer closed connection
		received += n;
	}

	return received;
}

