#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	fork();
	fork();
	execl("fork.out", "", NULL);
	execl("fork.out", "1", NULL);
	return 0;
}