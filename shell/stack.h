/*
 * Header file for a stack of character strings used to keep track of
 * directories in a simple shell.
 */

#ifndef _STACK_H_
#define _STACK_H_

struct stack_item {
	char *data;
	struct stack_item *next;
};

struct stack {
	struct stack_item *top;
};

struct stack *stack_init(void);
void stack_free(struct stack *s);

void push(struct stack *s, char *word);
char *pop(struct stack *s);
char *peek(struct stack *s);

void stack_print(struct stack *s);

struct stack_item *stack_item_init(char *word);
void stack_item_free(struct stack_item *item);

#endif
