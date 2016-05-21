#include "base.h"


#define WAIT_TTL 5

map<int, pair<sockaddr_storage, socklen_t> > peers;
set<int> finished_peers;

bool *life, *buf, *tlife;
int field_size;
handle socket_handle;


bool SendMessageAndField(int index, const message& msg, bool *life)
{
    socklen_t peer_addr_len = peers[index].second;
    sockaddr_storage peer = peers[index].first;
    
    if (sendto(socket_handle, &msg, sizeof(message), 0, 
        (const sockaddr*)&peer, peer_addr_len) != sizeof(message))
    {
        if (peers.find(index) != peers.end())
            peers.erase(index);
        perror("sendto");
        return false;
    }
    if (sendto(socket_handle, life, field_size * field_size, 0,
        (const sockaddr*)&peer, peer_addr_len) != field_size * field_size)
    {
        if (peers.find(index) != peers.end())
            peers.erase(index);
        perror("sendto");
        return false;
    }
    return true;
}

bool ReceiveField(int workers_count, bool* res_life)
{
    finished_peers.clear();
    int nread, response_length = field_size * field_size + sizeof(message);
    message msg;
    socklen_t peer_addr_len;
    sockaddr_storage peer;

    int wait_count = 0;

    for (int i = 0; i < workers_count; i++)
    {

        nread = recvfrom(socket_handle, buf, response_length, 
            MSG_DONTWAIT, (sockaddr*)&peer, &peer_addr_len);
        if (nread == -1 && errno == EAGAIN)
        {
            wait_count++;
            if (wait_count > WAIT_TTL)
                break;

            errno = 0;
            usleep(20 * 1000);
            i--;
            continue;
        }
        memcpy(&msg, buf, sizeof(message));
        memcpy(tlife + msg.from_p * field_size, buf + sizeof(message), nread - sizeof(message));
        finished_peers.insert(msg.id);

        cout << nread << " msg.id = " << msg.id << " msg.from = " << msg.from_p << " msg.to " << msg.to_p << endl;
    }

    if (finished_peers.size() == peers.size())
        return true;

    vector<int> diff;
    for (auto worker : peers)
        if (finished_peers.find(worker.first) == finished_peers.end())
            diff.push_back(worker.first);
    for (auto erase_elem : diff)
        peers.erase(erase_elem);

    return false;
}

void interrupt(int sig, siginfo_t *si, void* )
{
    message msg;
    memset(&msg, 0, sizeof(message));
    msg.size_f = -1;
    socklen_t peer_addr_len;
    sockaddr_storage peer;
    for (auto worker : peers)
    {
        peer = worker.second.first;
        peer_addr_len = worker.second.second;
        sendto(socket_handle, &msg, sizeof(message), 0,
         (const sockaddr*)&peer, peer_addr_len);
    }
    cout << endl;
    close(socket_handle);
    exit(0);
}

void Iteration()
{
    bool is_ok = true;
    message msg;
    int workers_count, cnt, part;
    do
    {
        cnt = 0;
        memcpy(tlife, life, field_size * field_size);
        workers_count = peers.size();

        if (workers_count == 0)
        {
            cout << "Not enough workers" << endl;
            exit(0);
        }
        part = field_size / workers_count;
        
        for (auto worker : peers)
        {
            msg.size_f = field_size;
            msg.from_p = cnt * part;
            if (cnt != workers_count - 1)
                msg.to_p = (cnt + 1) * part;
            else
                msg.to_p = field_size;
            msg.id = worker.first;
            cnt++;
            is_ok = SendMessageAndField(worker.first, msg, tlife);
            if (!is_ok)
                break;
        }

        if (!is_ok)
            continue;
      
        is_ok = ReceiveField(workers_count, tlife);
    } while (!is_ok);

    memcpy(life, tlife, field_size * field_size);
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        cout << "Usage as <size of field> <port> <number of workers>" << endl;
        return 0;
    }

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = interrupt;
    sigaction(SIGINT, &sa, NULL);

    field_size = atoi(argv[1]);
    life = new bool[field_size * field_size];
    tlife = new bool[field_size * field_size];
    buf = new bool[field_size * field_size + sizeof(message)];

    int horse_count = atoi(argv[3]);


    socket_handle = CreateListenSocket(string(argv[2]), SOCK_DGRAM);    
    socklen_t peer_addr_len;
    sockaddr_storage peer;
    message msg;
    memset(&msg, 0, sizeof(message));
        
    for (int i = 0; i < horse_count; i++)
    {
        peer_addr_len = sizeof(sockaddr_storage);
        recvfrom(socket_handle, &msg, sizeof(message), MSG_WAITALL, 
         (sockaddr*)&peer, &peer_addr_len);

        if (msg.size_f != -1)
        {
            cout << "Init error! msg.id != -1" << endl;
            i--;
            continue; 
        }
        peers[peers.size()] = make_pair(peer, peer_addr_len);
        cout << "Has peer! with id = " << i << endl;
    }


    for (int i = 0; i < field_size; i++)
    {
        for (int j = 0; j < field_size; j++)
        {
            set_cell(i, j, field_size, life, rand() % 2);
        }
    }
    

    for (;;)
    {
        cout << "in iteration" << endl;
        Iteration();
        usleep(200 * 1000);
        for (int i = 0; i < field_size; i++)
        {
            for (int j = 0; j < field_size; j++)
            {
                if (get_cell(i, j, field_size, life))
                    cout << ".";
                else
                    cout << " ";
            }
            cout << endl;
        }
        cout << endl << endl;
    }
    cout << "end!" << endl;

    return 0;
}