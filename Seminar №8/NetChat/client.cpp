#include "base.h"

int socket_fd;


void UserInput()
{
    string str;
    for (;;)
    {
        getline(cin, str);
        if (str == "q")
            return;
        SendData(socket_fd, str);
    }
}

void* ServerInput(void*)
{
    string str;
    bool fail = false;
    for (;;)
    {
        str = ReceiveData(socket_fd, fail);
        if (fail)
        {
            cout << "Server disconnected" << endl;
            close(socket_fd);
            exit(0);
        }
        cout << str << endl;   
    }
    return NULL;
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        cout << "Usage as <ip address> <port> <nickname>" << endl;
        return 0;
    }
    socket_fd = ConnectToHostPort(string(argv[1]), string(argv[2]), SOCK_STREAM);

    SendData(socket_fd, string(argv[3]));

    pthread_t temp;
    pthread_create(&temp, NULL, ServerInput, NULL);
    UserInput();
    close(socket_fd);
    return 0;
}