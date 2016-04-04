#include "Base.h"

int main()
{
    Dllhandle first, second;
    string first_name = buildName("FirstDll");
    string second_name = buildName("SecondDll");

    first = loadDll(first_name.c_str());
    second = loadDll(second_name.c_str());


    IPlugin *first_p, *second_p;
    
    auto first_dll = (init_dll)getFunction(first, "Init");
    first_p = first_dll();
    auto second_dll = (init_dll)getFunction(second, "Init");
    second_p = second_dll();

    first_p->PrintString("Hello, world!");
    second_p->PrintString("Hello, world!");

    auto first_rel = (release_plugin)getFunction(first, "Release");
    first_rel(first_p);
    auto second_rel = (release_plugin)getFunction(second, "Release");
    second_rel(second_p);


    closeDll(first);
    closeDll(second);

    getchar();
    return 0;
}
