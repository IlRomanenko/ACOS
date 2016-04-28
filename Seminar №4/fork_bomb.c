#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    fork();
    execl("fork.out", "fork.out", NULL);
    return 0;
}