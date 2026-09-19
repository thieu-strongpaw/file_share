CC = gcc
CFLAGS = -Wall -Wextra

SHARED_INCLUDE = -Ishared/include
SERVER_INCLUDE = -Iserver/include

SHARED_SRC = shared/src/recv_all.c \
						 shared/src/send_all.c

CLIENT_SRC = client/src/main.c $(SHARED_SRC) 
SERVER_SRC = server/src/main.c \
						 server/src/bind_addr.c \
						 $(SHARED_SRC)

CLIENT = client/src/fs_request
SERVER = server/src/fs_share

all: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $(SHARED_INCLUDE) $(CLIENT_SRC) -o $(CLIENT)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) $(SHARED_INCLUDE) $(SERVER_INCLUDE) $(SERVER_SRC) -o $(SERVER)

clean:
	rm -f $(CLIENT) $(SERVER)
