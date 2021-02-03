TARGET=server

all:
	gcc ${TARGET}.c -o ${TARGET} -lpthread
	gcc client.c -o client

flush:
	- rm -rf files
	mkdir files

clean:
	- rm server

