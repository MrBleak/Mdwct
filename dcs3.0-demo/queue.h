#ifndef BAIDU_DUER_DUERAPP_QUEUE_H
#define BAIDU_DUER_DUERAPP_QUEUE_H

#include <stdbool.h>


typedef void (*pfunc)();

#define QUEUE_TYPE pfunc

typedef enum{
	QUEUE_OK,
	QUEUE_ERROR
}RET;

typedef struct Qnode{
	QUEUE_TYPE data;
	struct Qnode *next;
}Qnode, *pQueue;

typedef struct {
	pQueue head;
	pQueue tail;
}LinkQueue;

RET duer_queue_Create(LinkQueue * que);
bool duer_queue_IsEmpty(LinkQueue * que);
RET duer_queue_Push(LinkQueue * que, const QUEUE_TYPE data);
RET duer_queue_Pop(LinkQueue * que, QUEUE_TYPE * e);
RET duer_queue_Delete(LinkQueue * que);

#endif // BAIDU_DUER_DUERAPP_QUEUE_H
