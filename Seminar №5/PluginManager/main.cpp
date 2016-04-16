#include "Base.h"


int main()
{
    auto plugins = ListOfPlugins();
    
    for (auto &name : plugins)
    {
        Dllhandle pluginHandle = loadDll(name.c_str());
        IPlugin *plugin;
        
        auto plugin_init_func = (init_dll)getFunction(pluginHandle, "Init");
        plugin = plugin_init_func();
        
        plugin->PrintString("Hello, world!");

        auto plugin_release_func = (release_plugin)getFunction(pluginHandle, "Release");
        plugin_release_func(plugin);
        closeDll(pluginHandle);
    }
    
    getchar();
    return 0;
}
