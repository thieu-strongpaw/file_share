CC = gcc
CFLAGS = -Wall -Wextra

SHARED_INCLUDE = -Ishared/include
SERVER_INCLUDE = -Iserver/include

SHARED_SRC = $(wildcard shared/src/*.c)
CLIENT_ONLY_SRC = $(wildcard client/src/*.c)
SERVER_ONLY_SRC = $(wildcard server/src/*.c)

CLIENT_SRC = $(CLIENT_ONLY_SRC) $(SHARED_SRC)
SERVER_SRC = $(SERVER_ONLY_SRC) $(SHARED_SRC)
						 

CLIENT = client/src/fs_request
SERVER = server/src/fs_share

all: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $(SHARED_INCLUDE) $(CLIENT_SRC) -o $(CLIENT)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) $(SHARED_INCLUDE) $(SERVER_INCLUDE) $(SERVER_SRC) -o $(SERVER)

clean:
	rm -f $(CLIENT) $(SERVER)
