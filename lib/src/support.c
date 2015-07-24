#include <stdlib.h>
#include "support.h"

void ms_list_init(ms_list_head * list)
{
    if (list == 0)
    {
        goto done;
    }
    list->head = 0;
    list->tail = 0;
    list->size = 0;

done:
    return;
}

void ms_list_add(ms_list_head * list, 
                list_element * toadd)
{
    if ((list == 0) || (toadd == 0))
    {
        goto done;
    }
    
    if (list->head == 0)
    {
        /* New list */
        list->head = toadd;
        list->tail = toadd;
        toadd->next = 0;
        list->size += 1;
    }
    else
    {
        /* Existing list */
        if (list->tail != 0)
        {
            list->tail->next = toadd;
            list->tail = toadd;
            toadd->next = 0;
            list->size += 1;
        }
    }
done:
    return;
}

void ms_list_free(ms_list_head * list)
{
    list_element * pElement = 0;
    list_element * pNext = 0;
    
    if (list == 0)
    {
        goto done;
    }
    if (list->size == 0)
    { 
        goto done;
    }
    pElement = list->head;
    while (pElement != 0)
    {
        pNext = pElement->next;
        free(pElement);
        pElement = 0;
        pElement = pNext;
    }
    
    list->head = 0;
    list->tail = 0;
    list->size = 0;
done:
    return; 
}
