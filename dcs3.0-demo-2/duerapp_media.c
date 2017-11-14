// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_media.c
 * Auth:
 * Desc: Duer Configuration.
 */

#include <gst/gst.h>
#include <assert.h>
#include <unistd.h>

#include "duerapp_media.h"
#include "duerapp_config.h"
#include "lightduer_dcs.h"

#define VOLUME_MAX (10.0)
#define VOLUME_MIX (0.000001)
#define VOLUME_INIT (2.0)

static duer_speak_state_t s_speak_state;
static duer_audio_state_t s_audio_state;
static pthread_t s_speak_thredID;
static pthread_t s_audio_thredID;
static GstElement *s_pip_speak;
static GstElement *s_pip_audio;
static GMainLoop *s_loop;
static char *s_url;
static int s_seek;
static double s_vol;
static bool s_mute;

void duer_media_init()
{
	s_speak_state = MEDIA_SPEAK_STOP;
	s_audio_state = MEDIA_AUDIO_STOP;
	s_pip_speak = NULL;
	s_pip_audio = NULL;
	s_url = NULL;
	s_loop = NULL;
	s_seek = 0;
	s_vol = VOLUME_INIT;
	s_mute = FALSE;
	gst_init(NULL, NULL);
}

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data)
{
	switch (GST_MESSAGE_TYPE (msg)) {

	case GST_MESSAGE_EOS:
		g_main_loop_quit(s_loop);
		break;

	case GST_MESSAGE_ERROR: {
			gchar  *debug;
			GError *error;

			gst_message_parse_error(msg, &error, &debug);
			g_free(debug);

			DUER_LOGE("gstreamer play : %s\n", error->message);
			g_error_free (error);

			g_main_loop_quit(s_loop);
		}
		break;
	default:
		break;
	}
}

static void speak_thread()
{
	// sleep(5);
	g_object_set(G_OBJECT (s_pip_speak), "volume", s_vol, NULL);
	GstBus *speak_bus = gst_pipeline_get_bus(GST_PIPELINE (s_pip_speak));
	s_loop = g_main_loop_new(NULL,FALSE);
	guint speak_bus_watch_id = gst_bus_add_watch (speak_bus, bus_call, s_loop);
	gst_object_unref(speak_bus);
	gst_element_set_state(s_pip_speak, GST_STATE_PLAYING);
	g_main_loop_run(s_loop);
	sleep(1000);
	gst_element_set_state (s_pip_speak, GST_STATE_NULL);

	g_source_remove(speak_bus_watch_id);
	gst_object_unref(GST_OBJECT (s_pip_speak));
	s_pip_speak = NULL;
	g_main_loop_unref(s_loop);
	s_loop = NULL;
	if (MEDIA_SPEAK_PLAY == s_speak_state) {
		s_speak_state = MEDIA_SPEAK_STOP;
		duer_dcs_speech_on_finished();
	}
}

void duer_media_speak_play(const char* url)
{
	if (MEDIA_SPEAK_STOP == s_speak_state) {
		s_pip_speak = gst_element_factory_make("playbin", "speak");
		assert(s_pip_speak != NULL);
		g_object_set(G_OBJECT (s_pip_speak), "uri", url, NULL);
		int ret = pthread_create(&s_speak_thredID, NULL, (void*)speak_thread, NULL);
		if(ret != 0)
		{
			DUER_LOGE("Create media speak pthread error!");
			exit(1);
		}
		s_speak_state = MEDIA_SPEAK_PLAY;
	} else {
		DUER_LOGE ("Speak play state : %d", s_speak_state);
	}
}

void duer_media_speak_stop()
{
	if (MEDIA_SPEAK_PLAY == s_speak_state) {
		s_speak_state = MEDIA_SPEAK_STOP;
		g_main_loop_quit(s_loop);
	} else {
		DUER_LOGE ("Speak stop state : %d", s_speak_state);
	}
}

void audio_thread()
{
	g_object_set(G_OBJECT (s_pip_audio), "volume", s_vol, NULL);
	GstBus *audio_bus = gst_pipeline_get_bus(GST_PIPELINE (s_pip_audio));
	s_loop = g_main_loop_new(NULL,FALSE);
	guint audio_bus_watch_id = gst_bus_add_watch (audio_bus, bus_call, s_loop);
	gst_object_unref(audio_bus);
	gst_element_set_state(s_pip_audio, GST_STATE_PLAYING);
	g_main_loop_run(s_loop);

	gst_element_set_state (s_pip_audio, GST_STATE_NULL);

	g_source_remove(audio_bus_watch_id);
	g_main_loop_unref(s_loop);
	s_loop = NULL;

	if (MEDIA_AUDIO_PLAY == s_audio_state) {
		gst_object_unref(GST_OBJECT (s_pip_audio));
		s_pip_audio = NULL;
		s_audio_state = MEDIA_AUDIO_STOP;
		duer_dcs_audio_on_finished();
		s_seek = 0;
	} else if (MEDIA_AUDIO_STOP == s_audio_state) {
		gst_object_unref(GST_OBJECT (s_pip_audio));
		s_pip_audio = NULL;
		s_seek = 0;
	}
}

void duer_media_audio_start(const char* url)
{
	if (MEDIA_AUDIO_STOP == s_audio_state) {
		s_pip_audio = gst_element_factory_make("playbin", "audio");
		assert(s_pip_audio != NULL);
		g_object_set(G_OBJECT (s_pip_audio), "uri", url, NULL);
		int ret = pthread_create(&s_audio_thredID, NULL, (void*)audio_thread, NULL);
		if(ret != 0)
		{
			DUER_LOGE ("Create media audio pthread error!");
			exit(1);
		}
		if (s_url) {
			free(s_url);
			s_url = NULL;
		}

		s_url = (char*)malloc(strlen(url) + 1);
		if (s_url) {
			strcpy(s_url, url);
		} else {
			DUER_LOGE ("malloc url failed!");
		}
		s_audio_state = MEDIA_AUDIO_PLAY;
	} else if (MEDIA_AUDIO_PAUSE == s_audio_state) {
		gst_object_unref(GST_OBJECT (s_pip_audio));
		s_pip_audio = NULL;
		s_audio_state = MEDIA_AUDIO_STOP;
		duer_media_audio_start(url);
	} else {
		DUER_LOGE ("Audio play state : %d", s_audio_state);
	}
}

void duer_media_audio_seek(const char* url, int offset)
{
	if (MEDIA_AUDIO_PAUSE == s_audio_state) {
		if ((strcmp(s_url, url) == 0) && (offset == s_seek)) {
			int ret = pthread_create(&s_audio_thredID, NULL, (void*)audio_thread, NULL);
			if(ret != 0)
			{
				DUER_LOGE ("Create media audio pthread error!");
				exit(1);
			}
		}
	} else if (MEDIA_AUDIO_STOP == s_audio_state) {
		duer_media_audio_start(url);
	} else {
		DUER_LOGE ("Audio seek state : %d", s_audio_state);
	}
}

void duer_media_audio_stop()
{
	if (MEDIA_AUDIO_PLAY == s_audio_state) {
		s_audio_state = MEDIA_AUDIO_STOP;
		g_main_loop_quit(s_loop);
	} else if (MEDIA_AUDIO_PAUSE == s_audio_state) {
		gst_object_unref(GST_OBJECT (s_pip_audio));
		s_pip_audio = NULL;
		s_audio_state = MEDIA_AUDIO_STOP;
	} else {
		DUER_LOGE ("Audio stop state : %d", s_audio_state);
	}
}

void duer_media_audio_pause()
{
	if (MEDIA_AUDIO_PLAY == s_audio_state) {
		s_audio_state = MEDIA_AUDIO_PAUSE;
		g_main_loop_quit(s_loop);
	} else {
		DUER_LOGE ("Audio pause state : %d", s_audio_state);
	}
}

int duer_media_audio_get_position()
{
	gint64 pos;
	 if (gst_element_query_position (s_pip_audio, GST_FORMAT_TIME, &pos)) {
		 s_seek = pos / GST_MSECOND;
	 }
	 return s_seek;
}

void duer_media_volume_change(int volume)
{
	if (s_mute) {
		return;
	}
	s_vol += volume / 10.0;
	if (s_vol < VOLUME_MIX) {
		s_vol = 0.0;
	} else if (s_vol > VOLUME_MAX){
		s_vol = VOLUME_MAX;
	}

	if (s_pip_speak) {
		g_object_set(G_OBJECT (s_pip_speak), "volume", s_vol, NULL);
	}

	if (s_pip_audio) {
		g_object_set(G_OBJECT (s_pip_audio), "volume", s_vol, NULL);
	}
	DUER_LOGI ("volume : %.1f", s_vol);
}

void duer_media_set_volume(int volume)
{
	if (s_mute) {
		return;
	}
	s_vol = volume / 10.0;
	if (s_pip_speak) {
		g_object_set(G_OBJECT (s_pip_speak), "volume", s_vol, NULL);
	}

	if (s_pip_audio) {
		g_object_set(G_OBJECT (s_pip_audio), "volume", s_vol, NULL);
	}
	DUER_LOGI ("volume : %.1f", s_vol);
}

int duer_media_get_volume()
{
	return (int)(s_vol * 10);
}

void duer_media_set_mute(bool mute)
{
	s_mute = mute;

	if (mute) {
		if (s_pip_speak) {
			g_object_set(G_OBJECT (s_pip_speak), "volume", 0.0, NULL);
		}

		if (s_pip_audio) {
			g_object_set(G_OBJECT (s_pip_audio), "volume", 0.0, NULL);
		}
	} else {
		if (s_pip_speak) {
			g_object_set(G_OBJECT (s_pip_speak), "volume", s_vol, NULL);
		}

		if (s_pip_audio) {
			g_object_set(G_OBJECT (s_pip_audio), "volume", s_vol, NULL);
		}
	}
}

bool duer_media_get_mute()
{
	return s_mute;
}
