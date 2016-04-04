#pragma once
#include <iostream>
#include <cstdio>
#include <string>
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

#else

#include <dlfcn.h>


typedef void* Dllhandle;
#define DllExt string(".so")
#define buildName(name) (string("./") + name + DllExt)

#define loadDll(name) dlopen(name, RTLD_LAZY)
#define getFunction(lib, name) dlsym(lib, name)
#define closeDll(lib) dlclose(lib)

#endif
