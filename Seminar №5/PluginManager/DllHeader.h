#pragma once
#include <iostream>
#include <string>
#include "IPlugin.h"

using namespace std;

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT 
#endif

extern "C"
{
 	DLL_EXPORT IPlugin* Init();
    DLL_EXPORT void Release(IPlugin*);
}