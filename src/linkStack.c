/*************************************************************************
	> File Name: linkStack.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月18日 星期五 20时22分05秒
 ************************************************************************/

#include "linkStack.h"
#include <stdlib.h>


pLink creat()
{
    node *p = NULL;
    p = malloc( sizeof(node) );
    p.data = 0;
    p.next = NULL;
    return p;
}

int push(pLink head, char e)
{
    if (!head)
        return -1;

    pLink p = head;

    while(p->next)
    {
        p = p->next;
    }

    p->next = malloc( sizeof(node) );
    if (!p->next)
        return -1;
    p->next->data = e;
    p->next->next = NULL;
    
    return 0;
}
