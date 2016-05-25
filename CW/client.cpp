#include "Base.h"
#include "network.h"

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
    if (argc != 3)
    {
        cout << "Usage as <ip address> <port>" << endl;
        return 0;
    }
    socket_fd = ConnectToHostPort(string(argv[1]), string(argv[2]), SOCK_STREAM);

    pthread_t temp;
    pthread_create(&temp, NULL, ServerInput, NULL);
    UserInput();
    close(socket_fd);
    return 0;
}