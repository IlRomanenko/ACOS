#include "base.h"

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        cout << "Usage as <ip address> <port>" << endl;
        return 0;
    }
    handle server_socket = ConnectToHostPort(string(argv[1]), string(argv[2]), SOCK_DGRAM);

    message msg;
    memset(&msg, 0, sizeof(message));
    msg.size_f = -1;
    write(server_socket, &msg, sizeof(message));

    int nread;

    cout << "msg sended" << endl;
    while(true)
    {
        cout << endl << endl;
        nread = read(server_socket, &msg, sizeof(message));
        if (nread == -1 || msg.size_f == -1)
        {
            cout << "Connection closed" << endl;
            break;
        }
        
        cout << nread << ' ' << msg.from_p << ' ' << msg.to_p << endl;
        if (nread != sizeof(message))
        {
            cout << "Error!" << endl;
            break;
        }
        int count = msg.size_f * msg.size_f;
        
        int response_length = msg.size_f * (msg.to_p - msg.from_p) + sizeof(message); 
        cout << "resp_length = " << response_length << endl;
        bool *buf = new bool[count];
        bool *res = new bool[response_length];
        memset(res, 0, response_length);
        memcpy(res, &msg, sizeof(message));
        res += sizeof(message);

        nread = read(server_socket, buf, count);
        cout << "read buf = " << nread << endl;

        if (nread != count)
        {
            cout << "Error!" << endl;
            break;
        }
        cout << "Ok"<< endl;


        bool *imag_begin = res - msg.size_f * msg.from_p;

        for (int i = msg.from_p; i < msg.to_p; i++)
            for (int j = 0; j < msg.size_f; j++)
                update_cell(i, j, msg.size_f, buf, imag_begin);

        cout << "before" << endl;
        for (int i = msg.from_p; i < msg.to_p; i++)
        {
            for (int j = 0; j < msg.size_f; j++)
                if (get_cell(i, j, msg.size_f, buf))
                    cout << ".";
                else
                    cout << " ";
            cout << endl;
        }
        cout << endl << endl << "after" << endl;
        for (int i = msg.from_p; i < msg.to_p; i++)
        {
            for (int j = 0; j < msg.size_f; j++)
                if (get_cell(i, j, msg.size_f, imag_begin))
                    cout << ".";
                else
                    cout << " ";
            cout << endl;
        }

        res -= sizeof(message);
        write(server_socket, res, response_length);
        delete buf;
        delete res;
    }

    close(server_socket);

    return 0;
}