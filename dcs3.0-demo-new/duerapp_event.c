// Copyright (2017) Baidu Inc. All rights reserved.
/**
 * File: event.c
 * Auth:
 * Desc: Duer Application Main.
 */

#include <stdio.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "duerapp_event.h"
#include "duerapp_config.h"
#include "lightduer_dcs.h"
#include "lightduer_voice.h"
#include "duerapp_recorder.h"
#include "duerapp_media.h"

#define VOLUME_STEP (5)

void event_play_puase()
{
	DUER_LOGV ("KEY_DOWN");
	static bool play_pause = false;
	if (play_pause) {
		duer_dcs_send_play_control_cmd(DCS_PLAY_CMD);
		play_pause = false;
	} else {
		duer_dcs_send_play_control_cmd(DCS_PAUSE_CMD);
		play_pause = true;
	}
}

void event_record_start()
{
	DUER_LOGV ("KEY_DOWN");
	duer_media_play_stop();
	duer_voice_start(16000);
	duer_increase_topic_id();
	duer_recorder_start();
	if (DUER_OK != duer_dcs_on_listen_started())
	{
		DUER_LOGE ("duer_dcs_on_listen_started failed!");
	}
}

void event_play_stop()
{
	DUER_LOGV ("KEY_DOWN");
  duer_media_play_stop();
}

void event_previous_song()
{
	DUER_LOGV ("KEY_DOWN");
	duer_media_play_stop();
	duer_dcs_send_play_control_cmd(DCS_PREVIOUS_CMD);
}

void event_next_song()
{
	DUER_LOGV ("KEY_DOWN");
	duer_media_play_stop();
	duer_dcs_send_play_control_cmd(DCS_NEXT_CMD);
}

void event_volume_incr()
{
	DUER_LOGV ("KEY_DOWN");
	duer_media_volume_change(VOLUME_STEP);

	if (DUER_OK == duer_dcs_on_volume_changed()) {
		DUER_LOGI ("repot volume change OK");
	}
}

void event_volume_decr()
{
	DUER_LOGV ("KEY_DOWN");
	duer_media_volume_change(-VOLUME_STEP);
	if (DUER_OK == duer_dcs_on_volume_changed()) {
		DUER_LOGI ("repot volume change OK");
	}
}

void event_volune_mute()
{
	static bool mute = false;
	DUER_LOGI ("KEY_DOWN : %d", mute);
	if (mute) {
		mute = false;
	} else {
		mute = true;
	}
	duer_media_set_mute(mute);
	duer_dcs_on_mute();
}

void duer_event_loop()
{
	struct termios kbd_ops, kbd_bak;
	int kbd_fd = 0;
	char kbd_event;
	bool loop_state = true;

	tcgetattr (kbd_fd, &kbd_ops);
	memcpy(&kbd_bak, &kbd_ops, sizeof(struct termios));
	kbd_ops.c_lflag &=~ (ICANON | ECHO);
	tcsetattr(kbd_fd, TCSANOW, &kbd_ops);

	while(loop_state) {
		if (read(kbd_fd, &kbd_event, 1) < 0) {
			continue;
		}
		switch ( kbd_event ) {
			case PLAY_PAUSE :
				event_play_puase();
				break;
			case RECORD_START :
				event_record_start();
				break;
			case PLAY_STOP :
				event_play_stop();
				break;
			case PREVIOUS_SONG :
				event_previous_song();
				break;
			case NEXT_SONG :
				event_next_song();
				break;
			case VOLUME_INCR :
				event_volume_incr();
				break;
			case VOLUME_DECR :
				event_volume_decr();
				break;
			case VOLUME_MUTE :
				event_volune_mute();
				break;
			case QUIT :
				loop_state = false;
				break;
			default :
				break;
		}
	}

	tcsetattr(kbd_fd, TCSANOW, &kbd_bak);
}
