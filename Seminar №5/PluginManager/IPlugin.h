#pragma once
#include <string>
using namespace std;


class IPlugin
{
protected:
    IPlugin() { }
public:
    virtual void PrintString(const char* str) = 0;
};
