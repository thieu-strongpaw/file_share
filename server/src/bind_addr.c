#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>

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
