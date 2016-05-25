#pragma once
#include <iostream>
#include <cstdio>
#include <fcntl.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <errno.h>
#include <vector>
#include <unistd.h>
#include <netdb.h>
#include <algorithm>
#include <map>
#include <signal.h>
#include <semaphore.h>
using namespace std;

typedef int handle;

handle ConnectToHostPort(string host, string port, int sock_type)
{
    handle socket_fd = -1;
    int rc;

    addrinfo hints, *result, *rp;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = sock_type;
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

handle CreateListenSocket(string port, int sock_type)
{
    handle socket_fd = -1;
    int rc;
    addrinfo hints, *result, *rp;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = sock_type;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    rc = getaddrinfo(NULL, port.c_str(), &hints, &result);

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
        if (bind(socket_fd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;
        close(socket_fd);
    }

    freeaddrinfo(result);
    return socket_fd;
}


string ReceiveData(int socket_fd, bool &has_failed)
{
    has_failed = false;
    int len, nread;
    nread = recv(socket_fd, &len, sizeof(int), MSG_WAITALL);
 
    if (nread == -1 || len == 0 || nread == 0)
    {
        has_failed = true;
        return "";
    }
 
    char* tmp = new char[len + 1];
    memset(tmp, 0, len + 1);
    tmp[len] = '\0';
    nread = recv(socket_fd, tmp, len, MSG_WAITALL);
    if (nread <= 0)
    {
        has_failed = true;
        return "";
    }
    if (nread < len)
    {
        cout << "Emmm..." << endl;
        cout << "nread = " << nread << endl;
    }
    
    string str(tmp);
    delete tmp;
    return str;
}

void SendData(int socket_fd, const string &str)
{
    int len = str.length();
    write(socket_fd, &len, sizeof(int));
    write(socket_fd, str.c_str(), len);   
}