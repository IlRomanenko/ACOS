#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    fork();
    execl(argv[0], argv[0], NULL);
    return 0;
}