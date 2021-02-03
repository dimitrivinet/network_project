

all:
	gcc server.c -o server -lpthread
	gcc client.c -o client

flush:
	- rm -rf files
	mkdir files

clean:
	- rm server
	- rm client

