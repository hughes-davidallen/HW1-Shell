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
	l->head = NULL;
	l->tail = NULL;
	return l;
}

/* free all data and links and then the list itself */
void list_free(struct list *l)
{
	struct link *cursor;
	struct link *old_cursor;

	cursor = l->head;
	while (cursor != NULL) {
		free(cursor->data);
		old_cursor = cursor;
		cursor = cursor->next;
		free(old_cursor);
	}
	free(l);
}

/* copy a word and append it to the end of the list */
void list_add(struct list *l, char *word)
{
	struct link *lin;

	lin = (struct link *) malloc(sizeof(struct link));
	lin->next = NULL;
	lin->data = strdup(word);

	if (l->tail == NULL) { /* special case: empty list */
		l->head = lin;
		l->tail = lin;
	} else {
		l->tail->next = lin;
		l->tail = lin;
	}
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
	} else if (cursor->next == NULL) { /*word is last item in list */
		l->tail = prev;
		prev->next = NULL;
	} else { /* word is found but not front of list */
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
