#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	pid_t pid;

	printf("Starting Simple Shell\n");

	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "fork failed\n");
		return -1;
	} else if (pid == 0) {
		printf("This is the child process %d\n", pid);
	} else {
		printf("This is the parent process %d\n", pid);
	}
	return EXIT_SUCCESS;
}
