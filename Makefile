clardis-server:
	g++ -std=gnu++11 -fno-strict-aliasing -fPIC -rdynamic -pthread -Wall -O2 -o clardis-server src/*.cpp -levent
	#g++ -std=gnu++11 -fPIC -rdynamic -fno-strict-aliasing -pthread -Wall -O2 -IDEPS/libevent/build/include -LDEPS/libevent/build/lib -o clardis-server src/*.cpp DEPS/libevent/build/lib/libevent.a -lrt

clean:
	rm -f clardis-server

all: clardis-server

run: clardis-server
	./clardis-server
