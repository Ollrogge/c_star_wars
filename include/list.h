#ifndef LIST_H
#define LIST_H

typedef struct list_head list_head_t;

struct list_head {
    list_head_t* next, *prev;
};

void list_add_front(list_head_t* head, list_head_t* entry);
void list_add_tail(list_head_t* head, list_head_t* entry);
void list_del(list_head_t* entry);
void list_init(list_head_t* list);

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#endif