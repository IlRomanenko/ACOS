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
#include <sys/wait.h>
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
#include <fstream>
#include <chrono>
#include <sstream>
using namespace std;

chrono::steady_clock mclock;

struct prog_rec
{
    string name;
    vector<string> args;
    int freq;
};

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

template<typename T> 
    void Print(const vector<T> &v)
{
    cout << endl << "Size = " << v.size() << endl;
    for (auto elem : v)
    {
        cout << "\t" << elem << endl;
    }
    cout << endl << endl;
}


vector<prog_rec> ReadConfig(string name)
{
    vector<prog_rec> result;

    fstream file;
    file.open(name.c_str(), ios_base::in);
    int n;
    prog_rec prog;
    file >> n;
    string targs;

    getline(file, targs);
    for (int i = 0; i < n; i++)
    {
        getline(file, targs);
        auto args = split(targs);
        prog.name = args[0];
        prog.args = args;
        file >> prog.freq;
        result.push_back(prog);
        getline(file, targs);
    }
    file.close();
    return result;
}

char** convertStringToCharArray(const vector<string> &v)
{
    char** res = new char*[v.size() + 1];
    for (size_t i = 0; i < v.size(); i++)
    {
        res[i] = new char[v[i].length() + 1];
        res[i][v[i].length()] = '\0';
        memcpy(res[i], v[i].c_str(), v[i].length());
    }
    res[v.size()] = NULL;
    return res;
}
