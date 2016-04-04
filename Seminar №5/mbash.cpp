#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>
#include <queue>
//#include <io.h>
#include <unistd.h>
using namespace std;


queue<string> split(const string &s)
{
    queue<string> res;
    string ts;
    for (auto &chr : s)
    {
        if (chr == ' ')
        {
            res.push(ts);
            ts = "";
        }
        else
            ts += chr;
    }
    res.push(ts);
    return res;
}

int main()
{
    string s;

    while (true)
    {
        getline(cin, s);
       // cout << s << endl;
        if (s == "exit")
            break;

        int res = fork();
        if (res == 0)
        {
        	//cout << "ok" << endl;
            auto argv = split(s);
            string name = argv.front();
            argv.pop();
            string params = name;
            while (!argv.empty())
            {
                s = argv.front();
                //cout << s << endl;
                argv.pop();
                if (s[0] == '<')
                {
                    FILE* file_in = fopen(s.substr(1).c_str(), "r");
                    int descr_in = fileno(file_in);
                    int dst_in = fileno(stdin);
                    fclose(stdin);
                    dup2(descr_in, dst_in);
                }
                else if (s[0] == '>')
                {
                    FILE* file_out = fopen(s.substr(1).c_str(), "w");
                    int descr_out = fileno(file_out);
                    int dst_out = fileno(stdout);
                    fclose(stdout);
                    dup2(descr_out, dst_out);
                }
                else
                    params += " " + s;
            }
           // cout << name << ' ' << params << endl << endl;
            execl(name.c_str(), params.c_str());
        }
    }

    return 0;
}