#ifndef _LIST_H_
#define _LIST_H_

struct link {
	struct link *next;
	char *data;
};

struct list {
	struct link *head;
	struct link *tail;
};

struct list *list_init(void);
void list_free(struct list *l);
int list_add(struct list *l, char *word);
int list_remove(struct list *l, char *word);
void list_print(struct list *l);

#endif
