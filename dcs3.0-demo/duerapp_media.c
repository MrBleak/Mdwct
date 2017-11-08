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
	MEDIA_PLAY_PAUSE,
	MEDIA_PLAY_STOP
}t_Media_Statue;

typedef struct{
	gboolean volumC;
	gboolean playSC;
}t_Change;

static t_Change change = {
	.volumC = FALSE,
	.playSC = FALSE
};

static char* s_url = NULL;
static t_Media_Statue mediaStatue = MEDIA_IDLE;
static double s_volume;
static pthread_t s_media_thredID;
static bool s_mute;
static GstElement* s_pipeline = NULL;
static GMainLoop* s_loop = NULL;
static gint64 s_pos = 0;
static t_PalyType play_type = PLAY_IDLE;
static int s_seek = 0;

#if 0
static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data) {
	if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_EOS) {
			DUER_LOGI ("\n\nInfo:media Play End\n");
			(*play_stop)();
			mediaStatue = MEDIA_PLAY_STOP;
	}
	return TRUE;
}
#endif

gboolean source_prepare_cb(GSource * source, gint * timeout) {
	*timeout = 200;
	return FALSE;
}

gboolean source_check_cb(GSource * source) {
	gint64 position, len;
  GstFormat format = GST_FORMAT_TIME;

	if ((mediaStatue == MEDIA_PLAY_START)
		&& gst_element_query_duration (s_pipeline, format, &len)
		&& (gst_element_query_position (s_pipeline, format, &position))) {

		g_print ("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
			GST_TIME_ARGS (position), GST_TIME_ARGS (len));
		if (position <= s_pos)	{
			DUER_LOGI ("media play end");
			mediaStatue = MEDIA_PLAY_STOP;
			s_pos = 0;
		}	else {
			s_pos = position;
		}
  }
	static double vol;
	static t_Media_Statue m_mediaState;

	if (((vol - s_volume) > VOLUME_MIX)
		|| ((vol - s_volume) < (-VOLUME_MIX))) {
		vol = s_volume;
		change.volumC = TRUE;
	}

	if (m_mediaState != mediaStatue) {
		m_mediaState = mediaStatue;
		change.playSC = TRUE;
	}

	return (change.volumC || change.playSC);
}

gboolean source_dispatch_cb(GSource * source,
							GSourceFunc callback,
							gpointer data) {
	if (change.volumC) {
		g_object_set(G_OBJECT (s_pipeline), "volume", s_volume, NULL);
		change.volumC = FALSE;
	}

	if (change.playSC) {
		if (MEDIA_PLAY_START == mediaStatue) {
			if (0 != s_seek) {
				gst_element_seek_simple(s_pipeline, GST_FORMAT_TIME,
					GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT,
					GST_MSECOND * s_seek);
				s_seek = 0;
			}
			g_object_set (G_OBJECT (s_pipeline), "volume", s_volume, NULL);
			g_object_set (G_OBJECT (s_pipeline), "uri", s_url, NULL);
			gst_element_set_state (s_pipeline, GST_STATE_PLAYING);
			DUER_LOGI ("media state change : %s", s_url);
		} else if(MEDIA_PLAY_PAUSE == mediaStatue) {
			gst_element_set_state (s_pipeline, GST_STATE_PAUSED);
		} else if(MEDIA_PLAY_STOP == mediaStatue)	{
			DUER_LOGI ("play end : %d", play_type);
			gst_element_set_state (s_pipeline, GST_STATE_READY);
			if (PLAY_SPEAK == play_type) {
				DUER_LOGV ("PLAY_SPEAK : play end");
				duer_dcs_speech_on_finished();
			} else if (PLAY_AUDIO == play_type) {
				DUER_LOGV ("PLAY_AUDIO : play end");
				duer_dcs_audio_on_finished();
			}
			play_type = PLAY_IDLE;
		}	else if(MEDIA_IDLE == mediaStatue) {
			g_main_loop_quit (s_loop);
		}
		change.playSC = FALSE;
	}
	return TRUE;
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

	g_object_set(G_OBJECT (s_pipeline), "volume", s_volume, NULL);

	g_main_loop_run (s_loop);

  gst_element_set_state (s_pipeline, GST_STATE_NULL);
  gst_object_unref (GST_OBJECT (s_pipeline));

	g_source_unref(source);

	g_main_loop_unref (s_loop);
}

void duer_media_init()
{
  if (MEDIA_IDLE == mediaStatue) {
    s_volume = VOLUME_MAX / 5;
		s_mute = false;
    int ret = pthread_create(&s_media_thredID, NULL, (void *)media_thread, NULL);
    if (ret != 0) {
      DUER_LOGE ("Create media pthread error!");
      exit(1);
    }
		mediaStatue = MEDIA_PLAY_STOP;
  } else {
    DUER_LOGE ("Error: Media init fail! mediaStatue:%d", mediaStatue);
  }
}

void duer_media_uninit()
{
	if (MEDIA_IDLE != mediaStatue) {
		mediaStatue = MEDIA_IDLE;
		pthread_join(s_media_thredID, NULL);
		if (s_url) {
			free(s_url);
			s_url = NULL;
		}
	} else {
		DUER_LOGE ("Error: Media uninit fail! mediaStatue:%d", mediaStatue);
	}
}

void duer_media_play_start(const char* url, t_PalyType pType)
{
	DUER_LOGV ("duer_media_play_start : %s", url);
  if (MEDIA_PLAY_STOP == mediaStatue) {
		if (s_url) {
			free(s_url);
			s_url = NULL;
		}
    s_url = (char*)malloc(strlen(url) + 1);
    strcpy(s_url, url);
		play_type = pType;
		mediaStatue = MEDIA_PLAY_START;
  } else {
    DUER_LOGE ("Error: Media Starting! mediaStatue:%d", mediaStatue);
  }
}

void duer_media_pause()
{
	if (MEDIA_PLAY_START == mediaStatue) {
		mediaStatue = MEDIA_PLAY_PAUSE;
	} else {
		DUER_LOGE ("Error: Media Play Pause Fail! mediaStatue:%d", mediaStatue);
	}
}

void duer_media_play_seek(const char* url, int offset, t_PalyType pType)
{
	DUER_LOGV ("media_play_seek : %s", url);
	if (MEDIA_PLAY_STOP == mediaStatue) {
		if (s_url) {
			free(s_url);
			s_url = NULL;
		}
		s_url = (char*)malloc (strlen(url) + 1);
		strcpy(s_url, url);
		play_type = pType;
		s_seek = offset;
	} else {
		DUER_LOGE ("Error: Media Starting! mediaStatue:%d", mediaStatue);
	}
}

void duer_media_play_stop()
{
	if ((MEDIA_PLAY_START == mediaStatue)
		|| (MEDIA_PLAY_PAUSE == mediaStatue)) {
		mediaStatue = MEDIA_PLAY_STOP;
	} else {
		DUER_LOGE ("Error: Media Play Stop Fail! mediaStatue:%d", mediaStatue);
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
