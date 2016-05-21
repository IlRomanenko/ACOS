LINK=g++
GPP=g++ -std=c++11 -Wall -O0 -g3 -c

build: server.o client.o
	$(LINK) server.o -o server
	$(LINK) client.o -o client	 

%.o: %.cpp
	$(GPP) $^ -o $@

clean:
	rm -rf *.o server client