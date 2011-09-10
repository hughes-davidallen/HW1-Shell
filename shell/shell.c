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
	const char *prompt = "sh>";
	char *line; /* location for entire input line */
	char *args[MAX_ARGS + 1]; /* individual arguments */
	int i; /* counters for parsing the arguments */
	size_t len;

	memset(args, 0, sizeof(args));

	printf("Starting Simple Shell %s\n%s", argv[0], prompt);

	while (1) {
		getline(&line, &len, stdin);

		args[0] = strtok(line, " \t\n");
		if (strcmp(args[0], "exit") == 0) {
			free(line);
			printf("sh: Goodbye\n\n");
			break;
		}

		for (i = 1; i <= MAX_ARGS; i++) {
			args[i] = strtok(NULL, " \t\n");
			if (args[i] == NULL)
				break;
		}

		pid = fork();
		if (pid < 0) {
			fprintf(stderr, "fork failed\n");
			return EXIT_FAILURE;
		} else if (pid == 0) {
		/*	printf("This is the child process %d\n", pid);*/
			execvp(args[0], args);

			/*
		 	* If execution reaches this point, the
		 	* exec system call has failed
		 	*/
			fprintf(stderr, "sh: unknown command: %s\n", args[0]);
			exit(1);
		} else {
			int rVal;
			while (pid != wait(&rVal))
				;
			printf("%s", prompt);
/*			printf("This is the parent process %d - %d\n", 
				pid, rVal); */
		}
	}
	return EXIT_SUCCESS;
}
