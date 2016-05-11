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



string readAll()
{
    string res = "", t;
    while (!cin.eof())
    {
        getline(cin, t);
        res += t + "\n";
    }
    return res;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        cout << "Wrong amount of arguments" << endl;
        cout << "Use as <hostname> <port>" << endl;
        exit(0);
    }

    string query = readAll();

    addrinfo hints, *result, *rp;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    int socket_fd = -1, rc = 0;

    rc = getaddrinfo(argv[1], argv[2], NULL, &result);
    if (rc != 0)
    {
        cout << "rc = " << rc << endl;
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
        cout << "Can't connect to " << argv[1] << endl;
        exit(0);
    }


    string addres(argv[1]);

    write(socket_fd, query.c_str(), query.length());

    int nread;
    while ((nread = recv(socket_fd, text, BUF_SIZE, MSG_WAITALL)) > 0)
    {
        for (int i = 0; i < nread; i++)
            putchar(text[i]);
    }

    return 0;
}