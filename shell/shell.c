#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define MAX_ARGS 32

int cd(char *path);

int main(int argc, char *argv[])
{
	pid_t pid;
	const char *prompt = "sh>";
	const char *delim = " \t\n";
	char *line; /* location for entire input line */
	char *args[MAX_ARGS + 1]; /* individual arguments */
	int i; /* counters for parsing the arguments */
	size_t len;

	memset(args, 0, sizeof(args));

	printf("Starting Simple Shell %s\n", argv[0]);

	while (1) { /* keep prompting until user 'exit's */
		printf("%s", prompt);

		getline(&line, &len, stdin); /* read all input at once */

		args[0] = strtok(line, delim);
		for (i = 1; i <= MAX_ARGS; i++) {
			args[i] = strtok(NULL, delim);
			if (args[i] == NULL)
				break;
		}

		/* Handle built-in commands */
		if (strcmp(args[0], "exit") == 0) {
			free(line);
			printf("sh: Goodbye\n\n");
			break;
		} else if (strcmp(args[0], "cd") == 0) {
			cd(args[1]);
			continue; /* don't fork a new process */
		} else if (strcmp(args[0], "pushd") == 0) {
			continue;
		} else if (strcmp(args[0], "popd") == 0) {
			continue;
		} else if (strcmp(args[0], "dirs") == 0) {
			continue;
		} else if (strcmp(args[0], "path") == 0) {
			continue;
		}

		pid = fork();
		if (pid < 0) {
			fprintf(stderr, "fork failed\n");
			return EXIT_FAILURE;
		} else if (pid == 0) {
		/*	printf("This is the child process %-d\n", pid);*/
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
		}
	}
	return EXIT_SUCCESS;
}

/*
 * Handles the cd command by makinga chdir system call.  Also interprets
 * errno in the event of a system call failure.
 */
int cd(char *path)
{
	int status;
	status = chdir(path); /* make a system call */
	if (status != 0) {
		fprintf(stderr, "sh: could not change directory\n");
		switch(errno) {
		case EACCES:
			fprintf(stderr, "\tPermission denied\n");
			break;
		case EFAULT:
			fprintf(stderr, "\tPath outside memory space\n");
			break;
		case EIO:
			fprintf(stderr, "\tIO error\n");
			break;
		case ELOOP:
			fprintf(stderr, "\tToo many symbolic links\n");
			break;
		case ENAMETOOLONG:
			fprintf(stderr, "\tPath name too long\n");
			break;
		case ENOENT:
			fprintf(stderr, "\tDirectory does not exist\n");
			break;
		case ENOMEM:
			fprintf(stderr, "\tInsufficient kernal memory\n");
			break;
		case ENOTDIR:
			fprintf(stderr, "\tNot a directory\n");
			break;
		default:
			break;
		}
	}

	return status;
}
