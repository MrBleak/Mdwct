/*************************************************************************
	> File Name: linkStack.h
	> Author: 
	> Mail: 
	> Created Time: 2017年08月18日 星期五 20时22分43秒
 ************************************************************************/

#ifndef _LINKSTACK_H
#define _LINKSTACK_H

typedef struct Node{
    char data;
    string Node* next;
}node, *pLink;


int push(pLink head, char e);
int pop(pLink head, char &e);
pLink creat();

#endif
