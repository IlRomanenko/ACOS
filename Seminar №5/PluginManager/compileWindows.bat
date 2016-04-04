g++ -shared -std=c++11 firstDll.cpp -o FirstDll.dll
g++ -shared -std=c++11 secondDll.cpp -o SecondDll.dll
g++ -std=c++11 main.cpp -o main.exe