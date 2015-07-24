#ifndef _MS_LIST_H
#define _MS_LIST_H

struct list_element;
struct list_element
{
    struct list_element * next;
};

typedef struct list_element list_element;

typedef struct ms_list_head
{
    list_element * head;
    list_element * tail;
    unsigned int size;
} ms_list_head;

void ms_list_init(ms_list_head * list);
void ms_list_add(ms_list_head * list, list_element * toadd);
void ms_list_free(ms_list_head * list);

#endif
