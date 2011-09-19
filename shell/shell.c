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

const char *msg = "sh:"; /* Prefix to all messages given by shell */
const char *ermsg = "error:"; /* Prefix for all error messages */
const char *prompt = "sh>"; /* displays when shell is ready */

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
		if (args[0] == NULL) {
			/* do nothing */
			continue;
		} else if (strcmp(args[0], "exit") == 0) {
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
			fprintf(stderr, "%s fork failed\n", ermsg);
			return EXIT_FAILURE;
		} else if (pid == 0) {
			execute(args);

			/*
			 * If execution reaches this point, the
			 * exec system call has failed
			 */
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
 * From here until the end of the file represents the internals of the shell.
 * This is where all of the built-in commands are handled.
 */

#include "list.h"
#include "stack.h"

#define MAX_PATH_LENGTH 100

struct list *path; /* linked list of search paths */
struct stack *dirstack; /* used to store old directory locations */
char *pwd;

/*
 * setup - initializes data structures used to keep track of the path
 * and directory stack
 */
void setup(void)
{
	path = list_init();

	dirstack = stack_init();

	pwd = getcwd(NULL, 0);
	if (pwd == NULL)
		fprintf(stderr, "%s can't get current directory\n", msg);
}

/*
 * cleanup - frees up malloced memory used to keep track of the path and
 * the directory stack
 */
void cleanup(void)
{
	list_free(path);
	stack_free(dirstack);
	free(pwd);
}

/*
 * cd - handles the cd command by making a chdir system call.  Also
 * interprets errno in the event of a system call failure.
 */
int cd(char *path)
{
	int status;
	status = chdir(path); /* make a system call */
	if (status != 0) {
		fprintf(stderr, "%s could not change directory\n", ermsg);
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
			fprintf(stderr, "\tDirectory %s does not exist\n",
								path);
			break;
		case ENOMEM:
			fprintf(stderr, "\tInsufficient kernal memory\n");
			break;
		case ENOTDIR:
			fprintf(stderr, "\t%s is not a directory\n", path);
			break;
		default:
			break;
		}
	}

	free(pwd); /* get rid of old value */
	pwd = getcwd(NULL, MAX_PATH_LENGTH);
	if (pwd == NULL)
		fprintf(stderr, "%s can't get current directory\n", msg);

	return status;
}

/*
 * pushd - push the current directory onto the directory stack and cd into
 * the directory given as path
 */
int pushd(char *path)
{
	int status;
	char *old_dir;

	old_dir = strdup(pwd);
	status = cd(path);
	if (status == 0)
		push(dirstack, old_dir);
	free(old_dir);

	return status;
}

/*
 * popd - pop the directory from the top of the directory stack if it
 * exists and cd into it
 */
int popd(void)
{
	int status;
	char *new_dir;

	if (dirstack->top == NULL) {
		fprintf(stderr, "%s Directory stack is empty\n", ermsg);
		return -1;
	}

	new_dir = peek(dirstack);
	status = cd(new_dir);
	if (status == 0)
		free(pop(dirstack));

	return status;
}

/*
 * dirs - prints the stack of the directories produced by pushd in reverse
 * order.  That is, the most recent directory appears on top
 */
void dirs(void)
{
	printf("%s\n", pwd);
	stack_print(dirstack);
}

/*
 * pathmanager - implements the path command, including adding a path to
 * the list, taking away a path from the list, and print the current
 * path list
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
				"\tpath [+|- /some/dir]\n", ermsg);
	}
}

/*
 * execute - called by child process after a fork to search through the
 * path and find the correct location of the program to run
 */
void execute(char *args[])
{
	char *loc;
	struct link *cursor;
	int tries;

	if (*args[0] == '/' || *args[0] == '.') {
		/*
		 * user has specified absolute or relative path so the
		 * path variable is ignored
		 */
		execv(args[0], args);
		return;
	}

	cursor = path->head;
	tries = 0;
	while (cursor != NULL) { /* iterate through all paths */
		int len;

		len = strlen(cursor->data) + strlen(args[0]) + 2;
		loc = (char *) malloc(len);
		if (loc != NULL) {
			memset(loc, 0, len);
			loc = strcat(loc, cursor->data);
			loc = strcat(loc, "/");
			loc = strcat(loc, args[0]);

			execv(loc, args);

			/* execv did not work, try the next path */
			free(loc);
			cursor = cursor->next;
		} else if (tries++ >= 3) {
			/*
			 * malloc has failed
			 * try three times and then give up
			 */
			fprintf(stderr, "%s could not execute command: %s\n",
							ermsg, args[0]);
			return;
		}
	}

	fprintf(stderr, "%s unknown command: %s\n", ermsg, args[0]);
	return; /* everthing operated correctly but command wasn't found */
}
