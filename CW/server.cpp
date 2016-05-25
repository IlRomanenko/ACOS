#include "Base.h"
#include "network.h"

sem_t *clients_semap;

ostream& operator << (ostream &stream, const prog_rec &rec)
{
    stream << "name : " << rec.name << endl << "args : ";
    for (auto elem : rec.args)
        stream << elem << ' ';
    stream << endl << "Frequency : " << rec.freq << endl << endl;
    return stream;
}

vector<prog_rec> programs;
vector<chrono::steady_clock::time_point> end_time;
vector<bool> has_ended;
map<int, int> pids;

void RunTask(size_t index)
{
    cout << "Run Task" << endl;

    has_ended[index] = false;
    end_time[index] = mclock.now() + chrono::seconds(programs[index].freq);
    char** args = convertStringToCharArray(programs[index].args);
    
    for (auto i : programs[index].args)
        cout << i <<  ' ';
    

    int pid = fork();
    if (pid == 0)
    {
        cout << "Pid = 0" << endl;
        execvp(programs[index].name.c_str(), args);
    }
    else
    {
        pids[index] = pid;
    }
    for (size_t i = 0; i < programs[index].args.size(); i++)
        delete args[i];
    delete args;
    return;
}

void Sheduler()
{
    int status;
    while (true)
    {
        usleep(10 * 1000);
        sem_wait(clients_semap);
        for (auto pr : pids)
        {
            if (waitpid(pr.second, &status, WNOHANG) > 0)
            {
                cout << "pid has waited" << endl;
                has_ended[pr.first] = true;
            }
        }
        auto cur_time = mclock.now();
        for (size_t i = 0; i < programs.size(); i++)
        {
            if (has_ended[i] && (cur_time - end_time[i]).count() > 0)
                RunTask(i);
        }
        sem_post(clients_semap);
    }
}

void* ClientHandler(void* data)
{
    int socket_fd = *((int*)data);
    bool fail;
    
    {
        stringstream response;
        response << "Good day. You are in small server sheduler." << endl;
        response << "Avaible commands: Help, Inf, Cfg <config file name>, Stop, Exit" << endl;
        SendData(socket_fd, response.str());
    }
    for(;;)
    {
        string str = ReceiveData(socket_fd, fail);
        cout << "str = " << str << endl;
        
        sem_wait(clients_semap);
        if (fail)
        {
            cout << "ClientConsole has left. Stopping..." << endl;
            exit(0);
        }
        auto req = split(str);
        Print(req);

        if (req[0] == "Inf")
        {
            stringstream response;
            
            response << "Total programs : " << programs.size() << endl << endl;
            int cnt = -1;
            for (auto pr : programs)
            {
                cnt++;
                response << "\t" << pr.name << endl << "\t";
                for (auto pr_args : pr.args)
                    response << pr_args << ' ';
                response << endl << "frequency : " << pr.freq << endl;
                response << "IsBusy : " << has_ended[cnt] << endl;
                response << "Pid : " << pids[cnt] << endl;
                response << endl << endl;
            }
            SendData(socket_fd, response.str());
        }
        else if (req[0] == "Cfg")
        {
            if (req.size() == 1)
            {
                SendData(socket_fd, "Please write <file name>");
            }
            else
            {
                auto name = req[1];
                cout << "cfg name = " << name << endl;

                programs = ReadConfig(name);
                has_ended.resize(programs.size(), true);
                end_time.resize(programs.size(), mclock.now());

                Print(programs);
            }
        }
        else if (req[0] == "Stop")
        {
            programs.clear();
            has_ended.clear();
            end_time.clear();
            cout << "Shedule has been removed" << endl;
        }
        else if (req[0] == "Exit")
        {
            close(socket_fd);
            cout << "Stopping..." << endl;
            exit(0);
        }
        else if (req[0] == "Help")
        {
            stringstream response;
            response << "Good day. You are in small server sheduler." << endl;
            response << "Avaible commands: Help, Inf, Cfg <config file name>, Stop, Exit" << endl;
            SendData(socket_fd, response.str());
        }
        else
        {
            SendData(socket_fd, "Please write \"Help\"");            
        }
        sem_post(clients_semap);
    }
}


void AcceptUserConsole(string port)
{
    int socket_fd = CreateListenSocket(port, SOCK_STREAM);
    int rc = listen(socket_fd, 10);  
    if (rc == -1)
    {
        perror("listen");
        exit(0);
    } 

    sockaddr_storage addr;
    socklen_t addr_size;
    auto client_socket = accept(socket_fd, (sockaddr*)&addr, &addr_size);

    pthread_t temp;
    int* pthread_socket = new int;
    *pthread_socket = client_socket;

    pthread_create(&temp, NULL, ClientHandler, pthread_socket);     
    close(socket_fd);
}

int main(int argc, char** argv)
{
    time(NULL);
    if (argc != 2)
    {
        cout << "Error! Usage as <port>" << endl;
        exit(0);
    }

    clients_semap = sem_open("/ilyasheduler", O_CREAT | O_EXCL, 0666, 1);
    if (clients_semap == SEM_FAILED)
    {
        sem_unlink("/ilyasheduler");
        clients_semap = sem_open("/ilyasheduler", O_CREAT, 0666, 1);
    }

    cout << "Wait for console user" << endl;
    AcceptUserConsole(string(argv[1]));

    cout << "User accepted. Go into Sheduler" << endl;
    Sheduler();

    return 0;
}
