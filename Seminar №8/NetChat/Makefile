LINK=g++ -lpthread
GPP=g++ -lpthread -std=c++11 -O0 -g3 -Wall -c

build: server.o client.o
	$(LINK) server.o -o server
	$(LINK) client.o -o client	 

%.o: %.cpp
	$(GPP) $^ -o $@

clean:
	rm -rf *.o server client