CC = gcc

all: server client

sercer: server.c
	$(CC) -o $@ $<

client: client.c
	$(CC) -o $@ $<

clean:
	rm server client
