#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <cstdio>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;


#define server_client "sc"
#define client_server "cs"


void SendData(int fout, const string& str)
{
    int len = str.length();
    write(fout, &len, sizeof(int));
    write(fout, str.c_str(), str.length());
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

struct thread_data
{
    const char *filename;
    thread_data (const char* str) : filename(str) { }
};

void* PendingData(void* data)
{
    const char *filename = ((thread_data*)data)->filename;
    cout << "Wait for connection" << endl;
    int fin = open(filename, O_RDONLY);
    cout << "Ready :)" << endl;
    string str;
    while (str != "Goodbye")
    {
        ReceiveData(fin, str);
        cout << "He : " << str << endl;
    }
    close(fin);
    delete (thread_data*)data;
    return NULL;
}

void* UserInput(void* data)
{
    const char* filename = ((thread_data*)data)->filename;
    cout << "Wait for connection" << endl;
    int fout = open(filename, O_WRONLY);
    cout << "Ready :)" << endl;
    string str;
    while (str != "Goodbye")
    {
        getline(cin, str);
        SendData(fout, str);
    }

    close(fout);
    delete (thread_data*)data;
    return NULL;
}

void Server()
{
    mkfifo((server_client), 0777); //as usual file
    mkfifo((client_server), 0777); //as usual file
    pthread_t temp;
    pthread_create(&temp, NULL, PendingData, new thread_data(client_server));
    UserInput(new thread_data(server_client));
}

void Client()
{
    mkfifo((server_client), 0777); //as usual file
    mkfifo((client_server), 0777); //as usual file
    pthread_t temp;
    pthread_create(&temp, NULL, UserInput, new thread_data(client_server));
    PendingData(new thread_data(server_client));
}

int main(int argc, char **argv)
{
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