// Copyright (2017) Baidu Inc. All rights reserved.
/**
 * File: queue.c
 * Auth:
 * Desc: Duer Application Main.
 */

#include "config.h"

RET duer_queue_Create(LinkQueue * que) {
	if (!que) {
	return QUEUE_ERROR;
	}

	RET _ret = QUEUE_OK;

	que->head = que->tail = (pQueue)malloc (sizeof(Qnode));

	if (!que->head) {
	_ret = QUEUE_ERROR;
	}
	return _ret;
}

bool duer_queue_IsEmpty(LinkQueue * que) {
	if (!que) {
	return QUEUE_ERROR;
	}
	bool _ret = false;
	if (que) {
		if (que->head == que->tail) {
			_ret = true;
		}
	}
	return _ret;
}

RET duer_queue_Push(LinkQueue * que, const QUEUE_TYPE data) {
	if (!que) {
		return QUEUE_ERROR;
	}
	pQueue p = (pQueue)malloc(sizeof(Qnode));
	if(!p) {
		return QUEUE_ERROR;
	}

	p->data = data;
	p->next = NULL;

	que->tail->next = p;
	que->tail = p;
	return QUEUE_OK;
}

RET duer_queue_Pop(LinkQueue * que, QUEUE_TYPE * e) {

	if (!que) {
		return QUEUE_ERROR;
	}

	if (que->tail == que->head) {
		return QUEUE_ERROR;
	}

	pQueue p = que->head->next;
	*e = p->data;
	que->head->next = p->next;
	if (que->tail == p) {
		que->tail = que->head;
	}

	free(p);

	return QUEUE_OK;
}

RET duer_queue_Delete(LinkQueue * que) {
	if (!que) {
		return QUEUE_OK;
	}

	while (que->head) {
		que->tail = que->head->next;
		free(que->head);
		que->head = que->tail;
	}

	return 0;
}
