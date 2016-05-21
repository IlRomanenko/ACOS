#pragma once
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
#include <errno.h>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <netdb.h>
#include <algorithm>
#include <map>
#include <signal.h>
#include <errno.h>
#include <set>
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

//first message from client to server (Undefined)
//from_p = to_p = -1 -> end of conversation
struct message
{
    int size_f, from_p, to_p, id;
};

const int offset[] = {-1, 0, 1};




bool get_cell(int x, int y, int size, bool *arr)
{
    return *(arr + x * size + y);
}

void set_cell(int x, int y, int size, bool *arr, bool what)
{
    *(arr + x * size + y) = what;
}

inline void update_cell(int x, int y, int size, bool *from, bool *to)
{
    int cnt = 0, tx, ty;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            tx = x + offset[i];
            if (tx >= size)
                tx = 0;
            else if (tx < 0)
                tx = size - 1;

            ty = y + offset[j];
            if (ty >= size)
                ty = 0;
            else if (ty < 0)
                ty = size - 1;

            if (tx == x && ty == y)
                continue;

            if (get_cell(tx, ty, size, from))
                cnt++;
        }
    }

    if (get_cell(x, y, size, from))
    {
        if (cnt == 2 || cnt == 3)
            set_cell(x, y, size, to, true);
        else
            set_cell(x, y, size, to, false);
    }
    else
    {
        if (cnt == 3)
            set_cell(x, y, size, to, true);
        else
            set_cell(x, y, size, to, false);
    }
} 