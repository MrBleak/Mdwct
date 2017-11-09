// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_media.c
 * Auth:
 * Desc: Duer Configuration.
 */

#include <string.h>
#include <gst/gst.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <gst/gst.h>

#include "duerapp_media.h"
#include "lightduer_dcs.h"

#define VOLUME_MAX (10.0)
#define VOLUME_MIX (0.000001)

typedef enum{
	MEDIA_IDLE,
	MEDIA_PLAY_START,
	MEDIA_PLAY_STOP
}duer_media_statue_t;

typedef struct{
	bool vol_change;
	bool play_change;
}duer_media_change_t;

static duer_media_change_t s_change = {
	.vol_change = FALSE,
	.play_change = FALSE
};

static char* s_url = NULL;
static duer_media_statue_t s_media_statue = MEDIA_IDLE;
static double s_volume;
static pthread_t s_media_thredID;
static bool s_mute;
static GstElement* s_pipeline = NULL;
static GMainLoop* s_loop = NULL;
static gint64 s_pos = 0;
static gint64 s_dur = 0;
static duer_play_type_t s_play_type = PLAY_IDLE;
static int s_seek = 0;

gboolean source_prepare_cb(GSource * source, gint * timeout) {
	*timeout = 200;
	return FALSE;
}

gboolean source_check_cb(GSource * source) {
	gint64 position, len;
  GstFormat format = GST_FORMAT_TIME;

	if ((s_media_statue == MEDIA_PLAY_START)
		&& gst_element_query_duration (s_pipeline, format, &len)
		&& (gst_element_query_position (s_pipeline, format, &position))) {

		g_print ("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
			GST_TIME_ARGS (position), GST_TIME_ARGS (len));

  }
	static double vol;
	static duer_media_statue_t save_state;

	if (((vol - s_volume) > VOLUME_MIX)
		|| ((vol - s_volume) < (-VOLUME_MIX))) {
		vol = s_volume;
		s_change.vol_change = TRUE;
	}

	if (save_state != s_media_statue) {
		save_state = s_media_statue;
		s_change.play_change = TRUE;
	}

	return (s_change.vol_change || s_change.play_change);
}

gboolean source_dispatch_cb(GSource * source,
							GSourceFunc callback,
							gpointer data) {
	if (s_change.vol_change) {
		g_object_set(G_OBJECT (s_pipeline), "volume", s_volume, NULL);
		s_change.vol_change = FALSE;
	}

	if (s_change.play_change) {
		if (MEDIA_PLAY_START == s_media_statue) {
			if (0 != s_seek) {
				gst_element_seek_simple(s_pipeline, GST_FORMAT_TIME,
					GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT,
					GST_MSECOND * s_seek);
				s_seek = 0;
			}
			g_object_set (G_OBJECT (s_pipeline), "volume", s_volume, NULL);
			g_object_set (G_OBJECT (s_pipeline), "uri", s_url, NULL);
			gst_element_set_state (s_pipeline, GST_STATE_PLAYING);
			DUER_LOGI ("media state s_change : %s", s_url);
		} else if(MEDIA_PLAY_STOP == s_media_statue)	{
			DUER_LOGI ("play end : %d", s_play_type);
			gst_element_set_state (s_pipeline, GST_STATE_READY);
			s_play_type = PLAY_IDLE;
		}	else if(MEDIA_IDLE == s_media_statue) {
			g_main_loop_quit (s_loop);
		}
		s_change.play_change = FALSE;
	}
	return TRUE;
}

static gboolean bus_call_back(GstBus *bus, GstMessage *msg, gpointer use_data)
{
	switch (GST_MESSAGE_TYPE (msg)) {
		case GST_MESSAGE_EOS: {
				if (PLAY_SPEAK == s_play_type) {
					DUER_LOGV ("PLAY_SPEAK : play end");
					duer_dcs_speech_on_finished();
				} else if (PLAY_AUDIO == s_play_type) {
					DUER_LOGV ("PLAY_AUDIO : play end");
					duer_dcs_audio_on_finished();
				}
				s_media_statue = MEDIA_PLAY_STOP;
			}
			break;
		case GST_MESSAGE_ERROR:
			s_media_statue = MEDIA_PLAY_STOP;
			DUER_LOGE ("duer merdia play error");
			break;
		default:
			break;
	}
}

void media_thread()
{
  gst_init(NULL,NULL);
  s_pipeline = gst_element_factory_make("playbin", "play");
  assert(s_pipeline != NULL);
	s_loop = g_main_loop_new(NULL,FALSE);

	GMainContext * maincontext;
	GSource * source;
	GSourceFuncs sourcefuncs = {
		sourcefuncs.prepare = source_prepare_cb,
		sourcefuncs.check = source_check_cb,
		sourcefuncs.dispatch = source_dispatch_cb,
		// sourcefuncs.finalize = source_finalize_cb
	};

	maincontext = g_main_loop_get_context(s_loop);
	source = g_source_new(&sourcefuncs, sizeof(GSource));
	g_source_attach(source, maincontext);
	maincontext = NULL;
	GstBus* bus = gst_pipeline_get_bus (GST_PIPELINE (s_pipeline));
	guint bus_id = gst_bus_add_watch (bus, bus_call_back, NULL);
	gst_object_unref (bus);

	g_object_set(G_OBJECT (s_pipeline), "volume", s_volume, NULL);

	g_main_loop_run (s_loop);

  gst_element_set_state (s_pipeline, GST_STATE_NULL);

	g_source_unref(source);
	g_source_remove (bus_id);
	g_main_loop_unref (s_loop);
	gst_object_unref (GST_OBJECT (s_pipeline));
}

void duer_media_init()
{
  if (MEDIA_IDLE == s_media_statue) {
    s_volume = VOLUME_MAX / 5;
		s_mute = false;
    int ret = pthread_create(&s_media_thredID, NULL, (void *)media_thread, NULL);
    if (ret != 0) {
      DUER_LOGE ("Create media pthread error!");
      exit(1);
    }
		s_media_statue = MEDIA_PLAY_STOP;
  } else {
    DUER_LOGE ("Error: Media init fail! s_media_statue:%d", s_media_statue);
  }
}

void duer_media_uninit()
{
	if (MEDIA_IDLE != s_media_statue) {
		s_media_statue = MEDIA_IDLE;
		pthread_join(s_media_thredID, NULL);
		if (s_url) {
			free(s_url);
			s_url = NULL;
		}
	} else {
		DUER_LOGE ("Error: Media uninit fail! s_media_statue:%d", s_media_statue);
	}
}

void duer_media_play_start(const char* url, duer_play_type_t play_type)
{
	DUER_LOGV ("duer_media_play_start : %s", url);
  if (MEDIA_PLAY_STOP == s_media_statue) {
		if (s_url) {
			free(s_url);
			s_url = NULL;
		}
    s_url = (char*)malloc(strlen(url) + 1);
    strcpy(s_url, url);
		s_play_type = play_type;
		s_media_statue = MEDIA_PLAY_START;
  } else {
    DUER_LOGE ("Error: Media Starting! s_media_statue:%d", s_media_statue);
  }
}

void duer_media_play_seek(const char* url,
			int offset,
			duer_play_type_t play_type)
{
	DUER_LOGV ("media_play_seek : %s", url);
	if (MEDIA_PLAY_STOP == s_media_statue) {
		if (s_url) {
			free(s_url);
			s_url = NULL;
		}
		s_url = (char*)malloc (strlen(url) + 1);
		strcpy(s_url, url);
		s_play_type = play_type;
		s_seek = offset;
	} else {
		DUER_LOGE ("Error: Media Starting! s_media_statue:%d", s_media_statue);
	}
}

void duer_media_play_stop()
{
	if (MEDIA_PLAY_START == s_media_statue) {
		s_media_statue = MEDIA_PLAY_STOP;
	} else {
		DUER_LOGE ("Error: Media Play Stop Fail! s_media_statue:%d", s_media_statue);
	}
}

void duer_media_volume_change(int volume)
{
	if (s_mute) {
		return;
	}
  double vol_tmp = volume / 10.0 + s_volume;

  if (vol_tmp < VOLUME_MIX) {
  	vol_tmp = 0.0;
  } else if (vol_tmp > VOLUME_MAX) {
  	vol_tmp = VOLUME_MAX;
  }
  s_volume = vol_tmp;
	DUER_LOGI ("volume : %.1f", s_volume);
}

void duer_media_set_volume(int volume)
{
	if (s_mute) {
		return;
	}
  s_volume = volume / 10.0;
}

int duer_media_get_volume()
{
  return (int)(s_volume * 10);
}

void duer_media_set_mute(bool mute)
{
	static double mute_bef = 0.0;
  s_mute = mute;
	if (mute) {
		if ((mute_bef < VOLUME_MIX)
			|| (mute_bef > (-VOLUME_MIX))) {
			mute_bef = s_volume;
			s_volume = 0.0;
		}
	} else {
		if ((s_volume < VOLUME_MIX)
			|| (s_volume > (-VOLUME_MIX))) {
			s_volume = mute_bef;
			mute_bef = 0.0;
		}
	}
}

bool duer_media_get_mute()
{
	return s_mute;
}

int duer_media_get_position()
{
	return s_pos / 1000000;
}
