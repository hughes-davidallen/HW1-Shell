#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include "shell.h"

#define MAX_ARGS 32

int main(int argc, char *argv[])
{
	pid_t pid; /* will be result of the fork() system call */
	const char *prompt = "sh>"; /* displays when shell is ready */
	const char *delim = " \t\n"; /* delimeters for argument parsing */
	char *line; /* location for entire input line */
	char *args[MAX_ARGS + 1]; /* individual arguments */
	int i; /* counter for parsing the arguments */
	size_t len; /* required by getline() function */

	memset(args, 0, sizeof(args));

	printf("Starting Simple Shell %s\n", argv[0]);
	setup(); /* set up the environment */

	while (1) { /* keep prompting until user 'exit's */
		printf("%s", prompt);

		getline(&line, &len, stdin); /* read all input at once */

		/* put all individual words into an array */
		args[0] = strtok(line, delim);
		for (i = 1; i <= MAX_ARGS; i++) {
			args[i] = strtok(NULL, delim);
			if (args[i] == NULL)
				break;
		}

		/* Handle built-in commands without forking */
		if (strcmp(args[0], "exit") == 0) {
			free(line);
			printf("sh: Goodbye\n\n");
			break;
		} else if (strcmp(args[0], "cd") == 0) {
			cd(args[1]);
			continue;
		} else if (strcmp(args[0], "pushd") == 0) {
			pushd(args[1]);
			continue;
		} else if (strcmp(args[0], "popd") == 0) {
			popd();
			continue;
		} else if (strcmp(args[0], "dirs") == 0) {
			dirs();
			continue;
		} else if (strcmp(args[0], "path") == 0) {
			pathmanager(args[1], args[2]);
			continue;
		}

		/* handle non built-ins by forking a new process */
		pid = fork();
		if (pid < 0) {
			fprintf(stderr, "sh: fork failed\n");
			return EXIT_FAILURE;
		} else if (pid == 0) {
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
	cleanup();

	return EXIT_SUCCESS;
}

/*
 * This section of the code represents the guts of the shell.  This is
 * where all of the built-in commands are handled.
 */

#include "stack.h"

#define MAX_PATH_LENGTH 100

struct stack *stack; /* used to store old directory locations */
char *path = "PATH=";
char *pwd;

/*
 * setup
 */
void setup(void)
{
	char **env; /* used to help clobber the PATH variable */

	/*
	 * Clobber the old PATH environment variable.  environ represents
	 * the implicit environment variables argument used by exec
	 */
	env = environ;
	while (*env != NULL) {
		if (strncmp(*env, "PATH", 4) == 0)
			*env = path;
		env++;
	}

	stack = stack_init();

	pwd = getcwd(NULL, MAX_PATH_LENGTH);
}

/*
 * cleanup
 */
void cleanup(void)
{
	stack_free(stack);
	free(pwd);
}

/*
 * Handles the cd command by making a chdir system call.  Also interprets
 * errno in the event of a system call failure.
 */
int cd(char *path)
{
	int status;
	status = chdir(path); /* make a system call */
	if (status != 0) {
		fprintf(stderr, "sh: could not change directory\n");
		switch (errno) {
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

	pwd = getcwd(NULL, MAX_PATH_LENGTH);
	if (pwd == NULL)
		fprintf(stderr, "sh: can't get current directory\n");

	return status;
}

/*
 * pushd
 */
int pushd(char *path)
{
	push(stack, pwd);
	return cd(path);
}

/*
 * popd
 */
int popd(void)
{
	if (stack->top == NULL) {
		fprintf(stderr, "sh: Directory stack is empty\n");
		return -1;
	}

	printf("%s\n", peek(stack));

	return cd(pop(stack));
}

/*
 * dirs
 */
void dirs(void)
{
	stack_print(stack);
}

/*
 * chuck - just until I can rename the path handler
 */
void pathmanager(char *symbol, char *dir)
{
	if (symbol == NULL) {
		printf("%s\n", path + 5); /* trim off 'PATH=' */
	} else if (*symbol == '+' && strlen(symbol) == 1) {
		if (dir != NULL) {
			int length;
			length = strlen(path) + strlen(dir) + 2;
			path = (char *) realloc(path, length);
			sprintf(path, "%s:%s", path, dir);
		}
	} else if (*symbol == '-' && strlen(symbol) == 1) {

	} else {

	}
}
