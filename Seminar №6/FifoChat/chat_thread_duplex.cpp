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
const char* fifo_name = "chatfifo";

sem_t *client_can_read, *server_can_read;

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

void check_sem_open(sem_t * semaphore)
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
    if (is_server) 
    {
        server_can_read = sem_open(server_read_name, 0, 0666, -1);
        check_sem_open(server_can_read);
    }
    else
    {
        client_can_read = sem_open(client_read_name, 0, 0666, -1);
        check_sem_open(client_can_read);
    }

    cout << "Ready :)" << endl;
    string str;
    while (str != "Goodbye")
    {
        if (is_server)
            sem_wait(server_can_read);
        else
            sem_wait(client_can_read);


        ReceiveData(fin, str);
        cout << "He : " << str << endl;
    }

    if (is_server)
        sem_close(server_can_read);
    else
        sem_close(client_can_read);
    
    close(fin);
    return NULL;
}

void* UserInput(void* data)
{
    int fout = open(fifo_name, O_RDWR);

    bool is_server = *((bool*)data);
    if (is_server) 
    {
        client_can_read = sem_open(client_read_name, 0666);
        check_sem_open(client_can_read);
    }
    else
    {
        server_can_read = sem_open(server_read_name, 0666);
        check_sem_open(server_can_read);
    }

    cout << "Ready :)" << endl;
    string str;
    while (str != "Goodbye")
    {
        getline(cin, str);
        SendData(fout, str);
        if (is_server)
            sem_post(client_can_read);
        else
            sem_post(server_can_read);
    }
    if (is_server)
        sem_close(client_can_read);
    else
        sem_close(server_can_read);
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
}

void Client()
{
    mkfifo(fifo_name, 0777); //as usual file
    pthread_t temp;
    bool* is_server = new bool;
    *is_server = false;
    pthread_create(&temp, NULL, PendingData, is_server);
    UserInput(is_server);
}

int main(int argc, char **argv)
{
    client_can_read = sem_open(client_read_name, O_CREAT, 0666, 0);
    sem_close(client_can_read);

    server_can_read = sem_open(server_read_name, O_CREAT, 0666, 0);
    sem_close(server_can_read);

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
    remove(client_server);
    remove(server_client);
    return 0;
}