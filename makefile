TARGET=server

all:
	gcc ${TARGET}.c -o ${TARGET} -lpthread

flush:
	- rm -rf files
	mkdir files

clean:
	- rm server

