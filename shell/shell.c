#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGS 32

int main(int argc, char *argv[])
{
	pid_t pid;
	char *args[MAX_ARGS + 1];
	int i, j; /* counters for parsing the arguments */
	char *line;
	size_t len;

	memset(args, 0, sizeof(args));
	printf("Starting Simple Shell\n>");

	i = 0;
	j = 0;

	getline(&line, &len, stdin);
	printf("%s\n", line);

	args[0] = strtok(line, " \t");
	for (i = 1; i <= MAX_ARGS; i++) {
		args[i] = strtok(NULL, " \t\n");
		if (args[i] == NULL)
			break;
	}

	for (i = 0; i <= MAX_ARGS; i++) {
		if (args[i] == NULL)
			break;
		printf("%s\n", args[i]);
	}

	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "fork failed\n");
		return EXIT_FAILURE;
	} else if (pid == 0) {
		printf("This is the child process %d\n", pid);
		execlp(args[0], args[0], args[1], args[2], NULL);

		/*
		 * If execution reaches this point, the
		 * exec system call has failed
		 */
		fprintf(stderr, "exec failed\n");
		exit(1);
	} else {
		int rVal;
		while (pid != wait(&rVal))
			;
		printf("This is the parent process %d - %d\n", pid, rVal);
	}
	return EXIT_SUCCESS;
}
