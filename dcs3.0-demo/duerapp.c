// Copyright (2017) Baidu Inc. All rights reserved.
/**
 * File: duerapp.c
 * Auth:
 * Desc: Duer Application Main.
 */

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lightduer_connagent.h"
// #include "lightduer_voice.h"
// #include "lightduer_play_event.h"
#include "duerapp_config.h"
#include "lightduer_dcs.h"

static bool s_started = false;
static pthread_t kbdThredID = NULL;

void duer_app_dcs_init()
{
	duer_dcs_framework_init();
	duer_dcs_voice_input_init();
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
{``
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

void loop()
{
	while (kbdThredID) {

	}
}

void kbd_thread()
{
	switch (scanKeyboard()) {
		case PLAY_PAUSE :
			{

			}
			break;
		case RECORD_START :
			{

			}
			break;
		case PLAY_STOP :
			{

			}
			break;
		case PRVIOUS_SONG :
			{

			}
			break;
		case NEXT_SONG :
			{

			}
			break;
		case VOLUME_INCR :
			{

			}
			break;
		case VOLUME_DECR :
			{

			}
			break;
		case RECONNTECT_CLOUD :
			{

			}
			break;
		case QUIT :
			{

			}
			break;
		default:
			break;
	}
}

int main(int argc, char* argv[])
{
	/* Check input arguments */
	if (argc != 2) {
		pfintf ("Usage: %s <profile>\n", argv[0]);
		return -1;
	}

	// init CA
	duer_initialize();

	// Set the Duer Event Callback
	duer_set_event_callback(duer_event_hook);

	int ret = pthread_create(&kbdThredID, NULL, (void *)kbd_thread, NULL);

	// try conntect baidu cloud
	duer_test_start();

	loop();

	return 0;
}
