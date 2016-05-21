LINK=g++ -lpthread
GPP=g++ -lpthread -std=c++11 -O0 -g3 -Wall -c

build: netcat.o
	$(LINK) netcat.o -o netcat	 

netcat.o: netcat.cpp
	$(GPP) netcat.cpp -c -o netcat.o

clean:
	rm -rf *.o netcat