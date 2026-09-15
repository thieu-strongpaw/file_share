#include <stdlib.h>
#include <sys/socket.h>

ssize_t send_all(int fd, void *buf, size_t buf_len, int flag)
{
	size_t sent = 0;
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
