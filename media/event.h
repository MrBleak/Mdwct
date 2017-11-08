// Copyright (2017) Baidu Inc. All rights reserved.
/**
 * File: event.c
 * Auth:
 * Desc: Duer Application Main.
 */

#ifndef BAIDU_DUER_DUERAPP_EVENT_H
#define BAIDU_DUER_DUERAPP_EVENT_H

#include "queue.h"


void event_Play_Puase();
void event_Record_Start();
void event_Play_Stop();
void event_Prvious_Song();
void event_Next_Puase();
void event_Volume_Incr();
void event_Volume_Decr();
void event_Reconntect_Cloud();
void event_Quit();

int event_queue_init();
void kbd_thread(LinkQueue* param);

void event_loop();

#endif // BAIDU_DUER_DUERAPP_EVENT_H
