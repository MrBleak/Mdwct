// Copyright (2017) Baidu Inc. All rights reserved.
/**
 * File: event.c
 * Auth:
 * Desc: Duer Application Main.
 */

#include "config.h"

extern const char* url;
static bool s_loop;

static LinkQueue* event_Queue = NULL;
pthread_t kbdThredID;

void event_Play_Puase()
{
	printf ("event_Play_Puase\n");
	static bool tmp = false;
	if (tmp) {
		media_Play_Pause();
		//duer_dcs_send_play_control_cmd(DCS_PLAY_CMD);
	} else {
		media_Play_Pause();
		//duer_dcs_send_play_control_cmd(DCS_PAUSE_CMD);
	}

}

void event_Record_Start()
{
	printf ("event_Record_Start,url:%s\n", url);
	media_Play_Start(url);
}

void event_Play_Stop()
{
	printf("event_Play_Stop\n");
}

void event_Prvious_Song()
{

}

void event_Next_Song()
{

}

void event_Volume_Incr() {
	media_Volume_Change(0.5);

}

void event_Volume_Decr() {
	media_Volume_Change(-0.5);

}

void event_Reconntect_Cloud() {


}

void event_Quit() {
	printf("event_Quit1\n");
	pthread_exit(NULL);
	s_loop = false;
	printf("event_Quit2\n");
}

void event_Volune_Mute() {
	static bool mute = false;
	if (mute) {
		mute = false;
	} else {
		mute = true;
	}
	media_Set_Mute(mute);

}

int event_queue_init()
{
	s_loop = true;
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
	while(s_loop) {
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
				duer_queue_Push(param, event_Next_Song);
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
			{
				printf("event_Quit1\n");
				//pthread_exit(NULL);
				s_loop = false;
				printf("event_Quit2\n");
			}
				break;
			case VOLUME_MUTE :
				duer_queue_Push(param, event_Volune_Mute);
				break;
			default:
				break;
		}
	}
}

void event_loop()
{
	while (1) {
		if (!duer_queue_IsEmpty(event_Queue)) {
			QUEUE_TYPE event = NULL;
			duer_queue_Pop(event_Queue, &event);
			if (event != NULL)
			{
				event();
				event = NULL;
			}
		}
		if (s_loop == false)
			break;
	}
}
