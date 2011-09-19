#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stack.h"

/* initializes an empty stack */
struct stack *stack_init(void)
{
	struct stack *s = (struct stack *) malloc(sizeof(struct stack));
	if (s != NULL)
		s->top = NULL;
	return s;
}

/* frees every item in the stack and then the stack itself */
void stack_free(struct stack *s)
{
	while (s->top != NULL)
		free(pop(s));
	free(s);
}

/*
 * push an item to the top of the stack
 * returns 1 on success and 0 on failure
 */
int push(struct stack *s, char *word)
{
	struct stack_item *new_item = stack_item_init(word);
	if (new_item == NULL)
		return 0;

	new_item->next = s->top;
	s->top = new_item;
	return 1;
}

/*
 * removes the word fromt the top of the stack and returns it
 * returns NULL if the stack is empty
 */
char *pop(struct stack *s)
{
	if (s->top != NULL) {
		char *word;
		struct stack_item *old_top;

		word = strdup(s->top->data);
		old_top = s->top;
		s->top = s->top->next;
		stack_item_free(old_top);

		return word;
	}

	/* the stack is empty */
	return NULL;
}

/*
 * returns the top word on the stack without removing it
 * returns NULL if the stack is empty
 */
char *peek(struct stack *s)
{
	if (s->top != NULL)
		return s->top->data;

	/* the stack is empty */
	return NULL;
}

/* prints the contents of the stack 1 per line, the top of the stack first */
void stack_print(struct stack *s)
{
	struct stack_item *cursor = s->top;
	while (cursor != NULL) {
		printf("%s\n", cursor->data);
		cursor = cursor->next;
	}
}

/* initializes a stack item and copies the word into a new location */
struct stack_item *stack_item_init(char *word)
{
	struct stack_item *new_item =
		(struct stack_item *) malloc(sizeof(struct stack_item));
	if (new_item != NULL)
		new_item->data = strdup(word);
	return new_item;
}

/* frees both the stack item and the word it contains */
void stack_item_free(struct stack_item *item)
{
	free(item->data);
	free(item);
}
