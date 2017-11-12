all: server client

server: server.o functions.o
	g++ server.o functions.o -o server

client: client.o
	g++ client.o -o client

server.o: server.cpp functions.h
	g++ -c server.cpp -o server.o

client.o: client.cpp
	g++ -c client.cpp -o client.o

functions.o: functions.cpp
	g++ -c functions.cpp -o functions.o

clean:
	rm *.o server client
