all: server client

server: server.o functions.o
	g++ server.o functions.o -o server

client: client.o
	g++ client.o -o client

clean:
	rm *.o server client
