g++ -shared -fPIC -std=c++11 firstDll.cpp -o Plugins/FirstDll.so
g++ -shared -fPIC -std=c++11 secondDll.cpp -o Plugins/SecondDll.so
g++ -std=c++11 -ldl main.cpp -o main