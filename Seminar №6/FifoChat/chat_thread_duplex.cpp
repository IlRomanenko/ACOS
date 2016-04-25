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


const char* client_read_name = "/clientcanread";
const char* server_read_name = "/servercanread";
const char* fifo_busy_name = "/chatfifobusy";
const char* fifo_name = "chatfifo";

sem_t *client_can_read, *server_can_read, *fifo_busy;

void SendData(int fout, const string& str)
{
    int len = str.length();
    write(fout, &len, sizeof(int));
    write(fout, str.c_str(), str.length() * sizeof(char));
}

void ReceiveData(int fin, string& str)
{
    int len = 0;
    read(fin, &len, sizeof(int));
    char *buf = new char[len];
    read(fin, buf, len);
    str = string(buf);
    delete buf;
}

void check_sem_open(sem_t* semaphore)
{
    if (semaphore == SEM_FAILED)
    {
        cout << strerror(errno) << endl;;
        exit(0);
    }
}

void* PendingData(void* data)
{
    int fin = open(fifo_name, O_RDWR);
    bool is_server = *((bool*)data);
    cout << "Ready :)" << endl;
    string str;
    while (str != "Goodbye")
    {
        if (is_server)
        {
            if (sem_wait(server_can_read) < 0)
                perror("sem_wait");
        }
        else
        {
            if (sem_wait(client_can_read) < 0)
                perror("sem_wait");
        }
        
        sem_wait(fifo_busy);

        ReceiveData(fin, str);
        cout << "He : " << str << endl;

        sem_post(fifo_busy);
    }

    /*if (is_server)
        sem_close(server_can_read);
    else
        sem_close(client_can_read);
    sem_close(fifo_busy);*/
    close(fin);
    return NULL;
}

void* UserInput(void* data)
{
    int fout = open(fifo_name, O_RDWR);
    bool is_server = *((bool*)data);

    cout << "Ready :)" << endl;
    string str;
    while (str != "Goodbye")
    {
        getline(cin, str);
        
        sem_wait(fifo_busy);

        SendData(fout, str);

        if (is_server)
            sem_post(client_can_read);
        else
            sem_post(server_can_read);

        sem_post(fifo_busy);
    }
    /*if (is_server)
        sem_close(client_can_read);
    else
        sem_close(server_can_read);
    sem_close(fifo_busy);*/
    close(fout);
    return NULL;
}

void Server()
{
    mkfifo(fifo_name, 0777); //as usual file
    pthread_t temp;
    bool* is_server = new bool;
    *is_server = true;
    pthread_create(&temp, NULL, PendingData, is_server);
    UserInput(is_server);
    pthread_cancel(temp);
}

void Client()
{
    mkfifo(fifo_name, 0777); //as usual file
    pthread_t temp;
    bool* is_server = new bool;
    *is_server = false;
    pthread_create(&temp, NULL, PendingData, is_server);
    UserInput(is_server);
    pthread_cancel(temp);
}

int main(int argc, char **argv)
{

    client_can_read = sem_open(client_read_name, O_CREAT | O_EXCL, 0777, 0);
    if (client_can_read == SEM_FAILED)
        client_can_read = sem_open(client_read_name, 0);
    check_sem_open(client_can_read);


    server_can_read = sem_open(server_read_name, O_CREAT | O_EXCL, 0777, 0);
    if (server_can_read == SEM_FAILED)
        server_can_read = sem_open(server_read_name, 0);
    check_sem_open(server_can_read);


    fifo_busy = sem_open(fifo_busy_name, O_CREAT, 0777, 1);
    if (fifo_busy == SEM_FAILED)
        fifo_busy = sem_open(fifo_busy_name, 0);
    check_sem_open(fifo_busy);
    
    if (argc != 2)
    {
        puts("Error! Arguments wasn't found\n");
        exit(-1);
    }
    string name_pr(argv[1]);

    if (name_pr == "server")
    {
        Server();
    }
    else if (name_pr == "client")
    {
        Client();
    }
    else
    {
        puts("Unknow argument\n");
        exit(-1);
    }
    
    return 0;
}