#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include "shell.h"
#include "list.h"

#define MAX_ARGS 32

const char *msg = "sh:"; /* Prefix to all messages given by shell */
const char *prompt = "sh>"; /* displays when shell is ready */
struct list *path; /* linked list of search paths */

int main(int argc, char *argv[])
{
	pid_t pid; /* will be result of the fork() system call */
	const char *delim = " \t\n"; /* delimeters for argument parsing */
	char *line; /* location for entire input line */
	char *args[MAX_ARGS + 1]; /* individual arguments */
	int i; /* counter for parsing the arguments */
	size_t len; /* required by getline() function */

	memset(args, 0, sizeof(args));

	printf("%s starting %s\n", msg, argv[0]);
	setup(); /* set up the environment */

	while (1) { /* keep prompting until user 'exit's */
		printf("%s", prompt);

		getline(&line, &len, stdin); /* read all input at once */

		/*
		 * put all individual words into an array. this also makes
		 * sure the list of args is null-terminated.
		 */
		args[0] = strtok(line, delim);
		for (i = 1; i <= MAX_ARGS; i++) {
			args[i] = strtok(NULL, delim);
			if (args[i] == NULL)
				break;
		}

		/* handle built-in commands without forking */
		if (strcmp(args[0], "exit") == 0) {
			free(line);
			printf("%s goodbye\n", msg);
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
			fprintf(stderr, "%s fork failed\n", msg);
			return EXIT_FAILURE;
		} else if (pid == 0) {
			char *loc;
			struct link *cursor;

			cursor = path->head;
			while (cursor != NULL) {

				loc = (char *) malloc(strlen(cursor->data)
						+ strlen(args[0]) + 2);
				loc = strcat(loc, cursor->data);
				loc = strcat(loc, "/");
				loc = strcat(loc, args[0]);

				printf("attempting to execute %s at location %s\n", args[0], loc);

				execv(loc, args);
				free(loc);
				cursor = cursor->next;
			}

			/*
			 * If execution reaches this point, the
			 * exec system call has failed
			 */
			fprintf(stderr, "%s unknown command: %s\n",
							msg, args[0]);
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

struct stack *dirstack; /* used to store old directory locations */
char *pwd;

/*
 * setup
 */
void setup(void)
{
	path = list_init();

	dirstack = stack_init();

	pwd = getcwd(NULL, MAX_PATH_LENGTH);
	if (pwd == NULL)
		fprintf(stderr, "%s can't get current directory\n", msg);
}

/*
 * cleanup
 */
void cleanup(void)
{
	list_free(path);
	stack_free(dirstack);
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
		fprintf(stderr, "%s could not change directory\n", msg);
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
		fprintf(stderr, "%s can't get current directory\n", msg);

	return status;
}

/*
 * pushd
 */
int pushd(char *path)
{
	int status;
	char *old_dir;

	old_dir = pwd;
	status = cd(path);
	if (status == 0)
		push(dirstack, old_dir);

	return status;
}

/*
 * popd
 */
int popd(void)
{
	if (dirstack->top == NULL) {
		fprintf(stderr, "%s Directory stack is empty\n", msg);
		return -1;
	}

	printf("%s\n", peek(dirstack));

	return cd(pop(dirstack));
}

/*
 * dirs
 */
void dirs(void)
{
	stack_print(dirstack);
}

/*
 * pathmanager
 */
void pathmanager(char *symbol, char *dir)
{
	if (symbol == NULL) {
		struct link *cursor;

		cursor = path->head;
		while (cursor != NULL) {
			printf("%s", cursor->data);
			cursor = cursor->next;

			if (cursor != NULL)
				printf(":");
			else
				printf("\n");
		}
	} else if (*symbol == '+' && strlen(symbol) == 1 && dir != NULL) {
		list_add(path, dir);
	} else if (*symbol == '-' && strlen(symbol) == 1 && dir != NULL) {
		list_remove(path, dir);
	} else {
		fprintf(stderr, "%s improper usage of path\n"
				"\tpath [+|- /some/dir]\n", msg);
	}
}
