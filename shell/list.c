/* Represents a singly linked list of strings */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"

/* initialize an empty linked list */
struct list *list_init(void)
{
	struct list *l;

	l = (struct list *) malloc(sizeof(struct list));
	if (l != NULL) {
		l->head = NULL;
		l->tail = NULL;
	}
	return l;
}

/* free all data and links and then the list itself */
void list_free(struct list *l)
{
	struct link *cursor;
	struct link *prev;

	cursor = l->head;
	while (cursor != NULL) {
		free(cursor->data);
		prev = cursor;
		cursor = cursor->next;
		free(prev);
	}
	free(l);
}

/*
 * copy a word and append it to the end of the list
 * returns 1 on success and 0 on failure
 */
int list_add(struct list *l, char *word)
{
	struct link *lin;

	lin = (struct link *) malloc(sizeof(struct link));
	if (lin == NULL)
		return 0;

	lin->next = NULL;
	lin->data = strdup(word);

	if (l->tail == NULL) { /* special case: empty list */
		l->head = lin;
		l->tail = lin;
	} else {
		l->tail->next = lin;
		l->tail = lin;
	}

	return 1;
}

/*
 * search the list for a word and remove it if found
 * return 1 if found and removed and 0 if not found
 */
int list_remove(struct list *l, char *word)
{
	struct link *cursor;
	struct link *prev;

	cursor = l->head;
	prev = NULL;
	while (cursor != NULL) {
		if (strcmp(word, cursor->data) == 0)
			break;

		prev = cursor;
		cursor = cursor->next;
	}

	if (cursor == NULL) { /* word not found in list */
		return 0;
	} else if (prev == NULL) { /* word is first item in list */
		l->head = cursor->next;
		if (cursor->next == NULL) /* word is only item in list */
			l->tail = NULL;
	} else if (cursor->next == NULL) { /*word is last item in list */
		l->tail = prev;
		prev->next = NULL;
	} else { /* word is found at middle of list */
		prev->next = cursor->next;
	}

	free(cursor->data);
	free(cursor);

	return 1;
}

/* print the contents of the list */
void list_print(struct list *l)
{
	struct link *cursor;

	cursor = l->head;
	while (cursor != NULL) {
		printf("%s\n", cursor->data);
		cursor = cursor->next;
	}
}
