#pragma once
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include "IPlugin.h"

using namespace std;

using init_dll = IPlugin*(*)(void);
using release_plugin = void(*)(IPlugin*);

#ifdef _WIN32

#include <Windows.h>

typedef HMODULE Dllhandle;
#define DllExt string(".dll")
#define buildName(name) (name + DllExt)

#define loadDll(name) LoadLibrary(name)
#define getFunction(lib, name) GetProcAddress(lib, name)
#define closeDll(lib) FreeLibrary(lib)

vector<string> ListOfPlugins()
{
    vector<string> res;
    HANDLE direc_handle;
    WIN32_FIND_DATA file;
    
    direc_handle = FindFirstFile("Plugins//*.dll", &file);
    if (direc_handle == INVALID_HANDLE_VALUE)
        return res;
    res.push_back("Plugins//" + string(file.cFileName));
    while (FindNextFile(direc_handle, &file))
    {
        res.push_back("Plugins//" + string(file.cFileName));
    }
    return res;
}


#else

#include <dlfcn.h>
#include <dirent.h>

typedef void* Dllhandle;
#define DllExt string(".so")
#define buildName(name) (string("./") + name + DllExt)

#define loadDll(name) dlopen(name, RTLD_LAZY)
#define getFunction(lib, name) dlsym(lib, name)
#define closeDll(lib) dlclose(lib)

vector<string> ListOfPlugins()
{
    vector<string> res;
    DIR *dir = opendir("Plugins");
    dirent *end;
    
    if (dir == NULL)
        return res;

    int len;
    while ((end = readdir(dir)) != NULL)
    {
    	len = strlen(end->d_name);
    	if (len > 2 && end->d_name[len - 3] == '.' && end->d_name[len - 2] == 's' && end->d_name[len - 1] == 'o')
    	{
    		res.push_back("Plugins/" + string(end->d_name));
    	}
    }
    return res;
}


#endif
