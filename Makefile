FLAGS = -Wall -g -Werror -std=gnu99
.PHONY: clean

all: server client

server: server.c socket.o protocol.o huffman.o hash.o minheap.o
	gcc $(FLAGS) -o server $^

client: client.c socket.o protocol.o huffman.o hash.o minheap.o
	gcc $(FLAGS) -o client $^

%.o: %.c socket.h hash.h minheap.h huffman.h
	gcc $(FLAGS) -c $<

clean:
	rm -f *.o server