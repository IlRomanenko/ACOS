#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <cstdio>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>


using namespace std;

sem_t *semap;

const char *semap_name = "/semap";

void check_sem_open(sem_t* semaphore)
{
    if (semaphore == SEM_FAILED)
    {
        cout << strerror(errno) << endl;;
        exit(0);
    }
}


int main(int argc, char** argv)
{

	const char* client_read_name = "/clientcanread";
	const char* server_read_name = "/servercanread";
	const char* fifo_busy_name = "/chatfifobusy";
	sem_unlink(client_read_name);
	sem_unlink(server_read_name);
	sem_unlink(fifo_busy_name);
	return 0;
}
