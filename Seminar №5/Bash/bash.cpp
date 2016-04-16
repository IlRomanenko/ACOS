#include <iostream>
#include <cstdio>
#include <string>
#include <string.h>
#include <sstream>
#include <queue>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
using namespace std;


vector<string> split(const string &s)
{
    vector<string> res;
    string ts;
    for (auto &chr : s)
    {
        if (chr == ' ')
        {
            if (ts != "")
                res.push_back(ts);
            ts = "";
        }
        else
            ts += chr;
    }
    res.push_back(ts);
    return res;
}

int main(int argc, char** m_argv)
{
    string s;
    getline(cin, s);
    auto argv = split(s);
    if (argv.size() == 0)
        exit(0);

    int count = 0;
    string file_in, file_out;

    for (int i = 0; i < (int)argv.size(); i++)
    {
        if (argv[i] == "|")
            count++;   
        if (argv[i][0] == '<')
        {
            file_in = argv[i].substr(1);
            argv.erase(argv.begin() + i);
            i--;
        }
        if (argv[i][0] == '>')
        {
            file_out = argv[i].substr(1);
            argv.erase(argv.begin() + i);
            i--;
        }
    }



    for (int i = 0; i < count * 2 - 1; i += 2)
    {
        int mas[2];
        pipe(mas);
        int res1 = fork();

        if (res1 != 0)
        {
            if (i == 0 && file_in != "")
            {
                int dst_in = fileno(stdin);
                int file_descr = open(file_in.c_str(), O_RDONLY);
                dup2(file_descr, dst_in);
            }
            int dst_out = fileno(stdout);
            close(mas[0]);
            dup2(mas[1], dst_out);
            execlp(argv[i].c_str(), argv[i].c_str(), NULL);   
        }
        else
        {
            int dst = fileno(stdin);
            close(mas[1]);
            dup2(mas[0], dst);
        }        
    }

    if (count == 0 && file_in != "") 
    {
        int dst_in = fileno(stdin);
        int file_descr = open(file_in.c_str(), O_RDONLY);
        dup2(file_descr, dst_in);
    }

    if (file_out != "")
    {
        int dst_out = fileno(stdout);
        FILE *fout = fopen(file_out.c_str(), "w");
        dup2(fileno(fout), dst_out);
    }
    execlp(argv.back().c_str(), argv.back().c_str(), NULL);
    return 0;
}