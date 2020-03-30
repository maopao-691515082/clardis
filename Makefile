clardis-server:
	g++ -std=gnu++11 -fno-strict-aliasing -Wall -O2 -o clardis-server src/*.cpp -levent

clean:
	rm -f clardis-server

all: clardis-server

run: clardis-server
	./clardis-server
