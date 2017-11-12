// Copyright (2017) Baidu Inc. All rights reserved.
/**
 * File: duerapp.c
 * Auth:
 * Desc: Duer Application Main.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lightduer_connagent.h"
#include "lightduer_voice.h"
#include "duerapp_config.h"
#include "lightduer_dcs.h"
#include "duerapp_recorder.h"
#include "duerapp_media.h"
#include "duerapp_event.h"

static bool s_started = false;

void duer_app_dcs_init() 
{
	duer_dcs_framework_init();
	duer_dcs_voice_input_init();
	duer_dcs_voice_output_init();
	duer_dcs_speaker_control_init();
	duer_dcs_audio_player_init();
}

static void duer_event_hook(duer_event_t *event)
{
    if (!event) {
        DUER_LOGE("NULL event!!!");
    }

    DUER_LOGD("event: %d", event->_event);
    switch (event->_event) {
    case DUER_EVENT_STARTED:
		{
			duer_app_dcs_init();
		    s_started = true;
        }
        break;
    case DUER_EVENT_STOPPED:
        s_started = false;
        break;
    }
}

void duer_test_start(const char* profile)
{
    const char *data = duer_load_profile(profile);
    if (data == NULL) {
        DUER_LOGE("load profile failed!");
        return;
    }

    DUER_LOGD("profile: \n%s", data);

    // We start the duer with duer_start(), when network connected.
    duer_start(data, strlen(data));

    free((void *)data);
}


int main(int argc, char* argv[])
{
	/* Check input arguments */
	if (argc != 2) {
		printf ("Usage: %s <profile>\n", argv[0]);
		return -1;
	}

	// init CA
	duer_initialize();

	// Set the Duer Event Callback
	duer_set_event_callback(duer_event_hook);

	// init media
	duer_media_init();

	// try conntect baidu cloud
	duer_test_start(argv[1]);

	duer_event_loop();

	duer_media_uninit();

	return 0;
}

void duer_dcs_stop_listen_handler(void) 
{
	duer_recorder_stop();
	duer_voice_stop();
}

void duer_dcs_speak_handler(const char *url) 
{
	duer_media_play_start(url, PLAY_SPEAK);
	DUER_LOGI ("SPEAK\turl:%s", url);
}

void duer_dcs_audio_play_handler(const char *url) 
{
	duer_media_play_start(url, PLAY_AUDIO);
	DUER_LOGI ("AUDIO\turl:%s", url);
}

void duer_dcs_get_speaker_state(int *volume, bool *is_mute) 
{
	*volume = duer_media_get_volume();
	*is_mute = duer_media_get_mute();
}

void duer_dcs_volume_set_handler(int volume) 
{
	duer_media_set_volume(volume);
}

void duer_dcs_volume_adjust_handler(int volume) 
{
	duer_media_volume_change(volume);
}

void duer_dcs_mute_handler(bool is_mute) 
{
	duer_media_set_mute(is_mute);
}

void duer_dcs_audio_stop_handler(void) 
{
	duer_media_play_stop();
}

void duer_dcs_audio_seek_handler(const char* url, int offset) 
{
	duer_media_play_seek(url, offset, PLAY_AUDIO);
}

int duer_dcs_audio_pause_handler(void)
{
	duer_media_play_stop();
	return duer_media_get_position();
}

