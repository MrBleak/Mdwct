#include "event.h"
#include <pthread.h>
#include "duerapp_kbd_listener.h"
#include <stdio.h>
#include <stdlib.h>
#include "duerapp_config.h"
#include "duerapp_recorder.h"
#include "lightduer_dcs.h"
#include "lightduer_voice.h"
#include "duerapp_media.h"

static LinkQueue* event_Queue = NULL;
pthread_t kbdThredID;

void event_Play_Puase()
{
	printf ("event_Play_Puase\n");
	static bool tmp = false;
	if (tmp) {
		media_Play_Pause();
		duer_dcs_send_play_control_cmd(DCS_PLAY_CMD);
	} else {
		media_Play_Pause();
		duer_dcs_send_play_control_cmd(DCS_PAUSE_CMD);
	}

}

void event_Record_Start()
{
	DUER_LOGI ("event_Record_Start\n");
	duer_voice_start(16000);
	duer_increase_topic_id();
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
	duer_dcs_send_play_control_cmd(DCS_PREVIOUS_CMD);
}

void event_Next_Song()
{
	DUER_LOGI ("event_Next_Puase\n");
	duer_dcs_send_play_control_cmd(DCS_NEXT_CMD);
}

void event_Volume_Incr() {
	media_Volume_Change(0.5);

	if (DUER_OK == duer_dcs_on_volume_changed()) {
		DUER_LOGI ("repot volume change OK");
	}
}

void event_Volume_Decr() {
	media_Volume_Change(-0.5);
	if (DUER_OK == duer_dcs_on_volume_changed()) {
		DUER_LOGI ("repot volume change OK");
	}
}

void event_Reconntect_Cloud() {
	DUER_LOGI ("event_Reconntect_Cloud\n");

}

void event_Quit() {
	DUER_LOGI ("event_Quit\n");
	pthread_cancel(kbdThredID);
}

void event_Volune_Mute() {
	static bool mute = false;
	if (mute) {
		mute = false;
	} else {
		mute = true;
	}
	media_Set_Mute(mute);
	duer_dcs_on_mute();
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
				duer_queue_Push(param, event_Quit);
				break;
			case VOLUME_MUTE :
				duer_queue_Push(param, event_Volune_Mute);
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
