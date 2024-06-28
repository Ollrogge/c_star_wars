#include "list.h"

static void _list_add(list_head_t* entry, list_head_t* prev, list_head_t *next) {
    next->prev = entry;
    entry->next = next;
    entry->prev = prev;
    prev->next = entry;
}

static void _list_del(list_head_t *entry, list_head_t* prev, list_head_t *next) {
    prev->next = next;
    next->prev = prev;
}

void list_add_front(list_head_t* head, list_head_t* entry) {
    _list_add(entry, head, head->next);
}

void list_add_tail(list_head_t* head, list_head_t* entry) {
    _list_add(entry, head->prev, head);
}

void list_del(list_head_t* entry) {
    _list_del(entry, entry->prev, entry->next);
}

void list_init(list_head_t* list) {
    list->next = list;
    list->prev = list;
}


