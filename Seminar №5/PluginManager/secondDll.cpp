#include "DllHeader.h"


class Plugin : public IPlugin
{
public:
    void PrintString(const char* str) override
    {
        cout << "Tonight we are young" << endl << str << endl;
    }
};

extern "C"
{
    DLL_EXPORT IPlugin* Init()
    {
        return new Plugin();
    }

    DLL_EXPORT void Release(IPlugin* obj)
    {
        delete obj;
    }
}