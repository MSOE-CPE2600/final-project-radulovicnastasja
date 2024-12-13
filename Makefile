CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGETS = chat_server chat_client

all: $(TARGETS)

chat_server: chat_server.c
	$(CC) $(CFLAGS) chat_server.c -o chat_server

chat_client: chat_client.c
	$(CC) $(CFLAGS) chat_client.c -o chat_client

clean:
	rm -f $(TARGETS)
