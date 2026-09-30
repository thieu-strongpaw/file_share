// server/src/handle_client.c

#include <arpa/inet.h>
#include <inttypes.h>
#include <stdint.h>
#include <sys/stat.h>

#include "recv_all.h"
#include "send_all.h"

#define MAX_FILE_NAME_LEN 255


int handle_client(int client_fd){
	// find the length of the file name
	uint32_t file_name_len; 
	ssize_t mess_len_check = recv_all(client_fd, &file_name_len, sizeof file_name_len, 0);

	if (mess_len_check == -1)
	{
		perror("recv_all did not receive message length");
		return -1;
	}

	if (mess_len_check != sizeof file_name_len)
	{
		fprintf(stderr, "client disconnected before filename length was received\n");
		return -1;
	}

	file_name_len = ntohl(file_name_len);

	// use the length of the file name to grab the rest of the file name from buffer
	if (file_name_len == 0 || file_name_len > MAX_FILE_NAME_LEN)
	{
		fprintf(stderr, "Invalid filename length\n");
		return -1;
	}

	char *file_name_buf = malloc(file_name_len + 1);
	if (file_name_buf == NULL)
	{
		perror("malloc failed");
		return -1;
	}

	ssize_t file_name_len_check = recv_all(client_fd, file_name_buf, file_name_len, 0);
	if (file_name_len_check == -1)
	{
		perror("recv_all did not receive file name.");
		free(file_name_buf);
		return -1;
	}

	if ((uint32_t)file_name_len_check != file_name_len)
	{
		fprintf(stderr, "client disconnected before full filename was received\n");
		free(file_name_buf);
		return -1;
	}
	
	file_name_buf[file_name_len] = '\0';
	printf("File name reads: %s\n", file_name_buf);
	
	// End handle_client()

	// handle_request() // Takes the file name, validates the file, send file.

	// We want to keep the requested files only coming from allowed directories.
	// For now, we will just prepend this file path 
	#define FS_SHARE_D_PATH "~/Documents/fs_share_d/"

	FILE *requested_file = fopen(file_name_buf, "rb");
	if (requested_file == NULL)
	{
		perror("fopen");
		return -1;
	}

	struct stat requested_file_info;

	if (fstat(fileno(requested_file), &requested_file_info) == -1)
	{
		perror("fstat");
		return -1;
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
		return -1;
	}


	size_t bytes_read;
	while ((bytes_read = fread(requested_file_buf, 1, sizeof requested_file_buf, requested_file)) > 0)
	{
		if (send_all(client_fd, requested_file_buf, bytes_read, 0) == -1)
		{
		perror("send_all file content failed");
		return -1;
		}	
	}

	if (ferror(requested_file))
	{
		perror("fread");
		return -1;
	}

	printf("Sending complete\n");

	// end handle_request()


	fclose(requested_file);
	free(file_name_buf);

	return 1;
}
