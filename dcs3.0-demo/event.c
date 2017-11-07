#include "event.h"
#include <pthread.h>
#include "duerapp_kbd_listener.h"
#include <stdio.h>
#include <stdlib.h>
#include "duerapp_config.h"

static LinkQueue* event_Queue = NULL;
pthread_t kbdThredID;

void event_Play_Puase()
{
	printf ("event_Play_Puase\n");
}

void event_Record_Start()
{
	DUER_LOGI ("event_Record_Start\n");
	duer_voice_start();
	recorder_Start();
	if (DUER_OK != duer_dcs_on_listen_started())
	{
		DUER_LOGE ("duer_dcs_on_listen_started failed!");
	}
}

void event_Play_Stop()
{
	DUER_LOGI ("event_Play_Stop\n");
}

void event_Prvious_Song()
{
	DUER_LOGI ("event_Prvious_Song\n");
}

void event_Next_Puase()
{
	DUER_LOGI ("event_Next_Puase\n");
}

void event_Volume_Incr()
{
	DUER_LOGI ("event_Volume_Incr\n");
}

void event_Volume_Decr()
{
	DUER_LOGI ("event_Volume_Decr\n");
}

void event_Reconntect_Cloud() {
	DUER_LOGI ("event_Reconntect_Cloud\n");

}

void event_Quit() {
	printf ("event_Quit\n");
	pthread_cancel(kbdThredID);
}

int event_queue_init()
{
	event_Queue = (LinkQueue*)malloc (sizeof(LinkQueue));
	if (!event_Queue) {
		goto ERR;
	}
	if (QUEUE_ERROR == duer_queue_Create(event_Queue)) {
		goto ERR;
	}
	if (0 != pthread_create(&kbdThredID, NULL, (void *)kbd_thread, event_Queue)) {
		goto ERR;
	}
	return 0;
ERR:
	return -1;
}

void kbd_thread(LinkQueue* param)
{
	while(1) {
		switch (scanKeyboard()) {
			case PLAY_PAUSE :
				duer_queue_Push(param, event_Play_Puase);
				break;
			case RECORD_START :
				duer_queue_Push(param, event_Record_Start);
				break;
			case PLAY_STOP :
				duer_queue_Push(param, event_Play_Stop);
				break;
			case PRVIOUS_SONG :
				duer_queue_Push(param, event_Prvious_Song);
				break;
			case NEXT_SONG :
				duer_queue_Push(param, event_Next_Puase);
				break;
			case VOLUME_INCR :
				duer_queue_Push(param, event_Volume_Incr);
				break;
			case VOLUME_DECR :
				duer_queue_Push(param, event_Volume_Decr);
				break;
			case RECONNTECT_CLOUD :
				duer_queue_Push(param, event_Reconntect_Cloud);
				break;
			case QUIT :
				duer_queue_Push(param, event_Quit);
				break;
			default:
				break;
		}
	}
}

void loop()
{
	while (kbdThredID) {
		pthread_testcancel();
		if (!duer_queue_IsEmpty(event_Queue)) {
			QUEUE_TYPE event = NULL;
			duer_queue_Pop(event_Queue, &event);
			if (event != NULL)
			{
				event();
			}
		}
	}
}
