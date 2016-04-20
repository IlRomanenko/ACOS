#include <iostream>
#include <cstdio>
#include <vector>
#include <dirent.h>
#include <map>
#include <algorithm>
#include <cstdlib>
using namespace std;

map<int, vector<pair<int, string> > > g;

void dfs(int vertex, string name, int tabs = 0)
{
    if (name != "")
    {
        for (int i = 0; i < tabs; i++)
            cout << "\t";
        cout << name << endl;
    }
    vector<pair<int, string> > vp = g[vertex];
    for (int i = 0; i < (int)vp.size(); i++)
        dfs(vp[i].first, vp[i].second, tabs + 1);
}

int main()
{
    char temp[1000];
    DIR *ent2, *dir = opendir("/proc/"), *subdir;
    struct dirent *ent;

    int pid, ppid;
    char c;

    while ((ent = readdir(dir)) != NULL)
    {
        if (ent->d_name[0] == '.')
            continue;
        if (!isdigit(ent->d_name[0]))
            continue;
        string str = "/proc/" + string(ent->d_name) + "/stat";

        FILE *file = fopen(str.c_str(), "r");
        if (file == NULL)
        {
            fclose(file);
            continue;
        }
        fscanf(file, "%d %s %c %d", &pid, temp, &c, &ppid);
        g[ppid].push_back(make_pair(pid, string(temp)));
        fclose(file);
    }

    dfs(0, "", -1);

    return 0;
}