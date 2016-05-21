#include <iostream>
#include <cstdio>
#include <fcntl.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
using namespace std;

void check_error(int rc)
{
    if (rc == -1)
    {
        cout << strerror(errno) << endl;
        exit(0);
    }
}

const int BUF_SIZE = 1024;

char text[BUF_SIZE];
int socket_fd;

int ConnectToHostPort(string host, string port)
{
    int socket_fd = -1, rc;
    
    addrinfo hints, *result, *rp;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    rc = getaddrinfo(host.c_str(), port.c_str(), &hints, &result);

    if (rc != 0)
    {
        cout << "getaddrinfo: " << gai_strerror(rc) << endl;
        exit(-1);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        socket_fd = socket(rp->ai_family, rp->ai_socktype,
                    rp->ai_protocol);
        if (socket_fd == -1)
            continue;
        if (connect(socket_fd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;
        close(socket_fd);
    }

    freeaddrinfo(result);
    if (socket_fd == -1)
    {
        cout << "Can't connect to " << host << " with port = " << port << endl;
        exit(-1);
    }

    return socket_fd;
}

void* UserInput(void*)
{
    string str;
    while (true)
    {
        getline(cin, str);
        str += "\n";
        write(socket_fd, str.c_str(), str.length());
    }
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        cout << "Wrong amount of arguments" << endl;
        cout << "Use as <hostname> <port>" << endl;
        exit(0);
    }

    socket_fd = ConnectToHostPort(string(argv[1]), string(argv[2]));

    pthread_t temp;

    pthread_create(&temp, NULL, UserInput, NULL);

    int nread;
    while ((nread = recv(socket_fd, text, BUF_SIZE, 0)) > 0)
    {
        for (int i = 0; i < nread; i++)
            putchar(text[i]);
    }

    return 0;
}