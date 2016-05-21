#include "base.h"

vector<pair<int, string> > clients;

sem_t *clients_semap;

void* ClientHandler(void* data)
{
    string my_nickname;
    int socket_fd = *((int*)data);
    
    sem_wait(clients_semap);
    for (size_t i = 0; i < clients.size(); i++)
        if (clients[i].first == socket_fd)
            my_nickname = clients[i].second;
    sem_post(clients_semap);

    bool disconnect = false;

    for (;;)
    {
        string str = ReceiveData(socket_fd, disconnect);

        if (disconnect)
        {
            sem_wait(clients_semap);
            for (size_t i = 0; i < clients.size(); i++)
            {
                if (clients[i].first != socket_fd)
                {
                    SendData(clients[i].first, my_nickname + " has left chat room");
                }
            }
            for (size_t i = 0; i < clients.size(); i++)
                if (clients[i].first == socket_fd)
                {
                    swap(clients[i], clients.back());
                    clients.pop_back();
                }
            sem_post(clients_semap);
            break;
        }

        sem_wait(clients_semap);
        for (size_t i = 0; i < clients.size(); i++)
        {
            if (clients[i].first != socket_fd)
            {
                SendData(clients[i].first, my_nickname + " : " + str);
            }
        }
        sem_post(clients_semap);
    }
    return NULL;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cout << "Usage as <port>" << endl;
        return 0;
    }

    clients_semap = sem_open("/chatroom", O_CREAT | O_EXCL, 0666, 1);
    if (clients_semap == SEM_FAILED)
        clients_semap = sem_open("/chatroom", 0, 0666, 1);

    pthread_t temp;
    int socket_fd = CreateListenSocket(argv[1], SOCK_STREAM);
    int rc = listen(socket_fd, 10);  
    if (rc == -1)
    {
        perror("listen");
        exit(0);
    } 
    for (;;)
    {
        sockaddr_storage addr;
        socklen_t addr_size;
        int* pth_socket_fd = new int;
        rc = accept(socket_fd, (sockaddr*)&addr, &addr_size);
        if (rc == -1)
        {
            cout << "Error with accept" << endl;
            perror("accept");
            exit(0);
        }
        bool fail = false;
        string nickname = ReceiveData(rc, fail);
        cout << "Added user nickname = " << nickname << endl;

        if (fail)
            continue;


        sem_wait(clients_semap);        

        for (size_t i = 0; i < clients.size(); i++)
            SendData(clients[i].first, nickname + " has joined");
        clients.push_back(make_pair(rc, nickname));
        
        sem_post(clients_semap);

        *pth_socket_fd = rc;
        pthread_create(&temp, NULL, ClientHandler, pth_socket_fd);     
    }

    return 0;
}