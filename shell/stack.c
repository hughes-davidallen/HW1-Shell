#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stack.h"

struct stack_item *stack_item_init(char *word);
struct stack_item *stack_item_free(struct stack_item *item);

struct stack *stack_init(void)
{
	struct stack *s = (struct stack *) malloc(sizeof(struct stack));
	s->top = NULL;
	return s;
}

void stack_free(struct stack *s)
{
	while (s->top != NULL) {
		free(s->top->data);
		pop(s);
	}
	free(s);
}

void push(struct stack *s, char *word)
{
	struct stack_item *new_item = stack_item_init(word);
	new_item->next = s->top;
	s->top = new_item;
}

char *pop(struct stack *s)
{
	char *word = s->top->data;
	s->top = stack_item_free(s->top);
	return word;
}

char *peek(struct stack *s)
{
	return s->top->data;
}

void stack_print(struct stack *s)
{
	struct stack_item *cursor = s->top;
	while (cursor != NULL) {
		printf("%s\n", cursor->data);
		cursor = cursor->next;
	}
}

struct stack_item *stack_item_init(char *word)
{
	struct stack_item *new_item =
		(struct stack_item *) malloc(sizeof(struct stack_item));
	new_item->data = strdup(word);
	return new_item;
}

struct stack_item *stack_item_free(struct stack_item *item)
{
	struct stack_item *next = item->next;
	free(item);
	return next;
}
