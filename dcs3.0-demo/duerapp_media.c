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

static char* url = NULL;
static t_Media_Statue mediaStatue = MEDIA_IDLE;
static double volume;
static pthread_t mediaThredID;
static bool mute;
static GstElement* pipeline = NULL;
static GMainLoop* loop = NULL;
static gint64 pos = 0;
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
	gint64 m_pos, len;
  GstFormat format = GST_FORMAT_TIME;

	if ((mediaStatue == MEDIA_PLAY_START)
		&& gst_element_query_duration (pipeline, format, &len)
		&& (gst_element_query_position (pipeline, format, &m_pos))) {

		g_print ("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
			GST_TIME_ARGS (m_pos), GST_TIME_ARGS (len));
		if (m_pos <= pos)	{
			DUER_LOGI ("\nInfo:media Play End\n");
			mediaStatue = MEDIA_PLAY_STOP;
			pos = 0;
		}	else {
			pos = m_pos;
		}

  }
	static double m_volume;
	static t_Media_Statue m_mediaState;

	if (((m_volume - volume) > VOLUME_MIX)
		|| ((m_volume - volume) < (-VOLUME_MIX))) {
		m_volume = volume;
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
		g_object_set(G_OBJECT (pipeline), "volume", volume, NULL);
		change.volumC = FALSE;
	}

	if (change.playSC) {
		if (MEDIA_PLAY_START == mediaStatue) {
			if (0 != s_seek) {
				gst_element_seek_simple(pipeline, GST_FORMAT_TIME,
					GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT,
					GST_MSECOND * s_seek);
				s_seek = 0;
			}
			g_object_set (G_OBJECT (pipeline), "volume", volume, NULL);
			g_object_set (G_OBJECT (pipeline), "uri", url, NULL);
			gst_element_set_state (pipeline, GST_STATE_PLAYING);
			DUER_LOGI ("\nmedia state change : %s\n", url);
		} else if(MEDIA_PLAY_PAUSE == mediaStatue) {
			gst_element_set_state (pipeline, GST_STATE_PAUSED);
		} else if(MEDIA_PLAY_STOP == mediaStatue)	{
			DUER_LOGI ("play end : %d\n", play_type);
			gst_element_set_state (pipeline, GST_STATE_READY);
			if (PLAY_SPEAK == play_type) {
				DUER_LOGI ("PLAY_SPEAK : play end");
				duer_dcs_speech_on_finished();
			} else if (PLAY_AUDIO == play_type) {
				DUER_LOGI ("PLAY_AUDIO : play end");
				duer_dcs_audio_on_finished();
			}
			play_type = PLAY_IDLE;
		}	else if(MEDIA_IDLE == mediaStatue) {
			g_main_loop_quit (loop);
		}
		change.playSC = FALSE;
	}
	return TRUE;
}

void media_thread() {
  gst_init(NULL,NULL);
  pipeline = gst_element_factory_make("playbin", "play");
  assert(pipeline!=NULL);
	loop = g_main_loop_new(NULL,FALSE);

#if 0
	GstBus* bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	guint bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
	gst_object_unref (bus);
#endif

	GMainContext * maincontext;
	GSource * source;
	GSourceFuncs sourcefuncs = {
		sourcefuncs.prepare = source_prepare_cb,
		sourcefuncs.check = source_check_cb,
		sourcefuncs.dispatch = source_dispatch_cb,
		// sourcefuncs.finalize = source_finalize_cb
	};

	maincontext = g_main_loop_get_context(loop);
	source = g_source_new(&sourcefuncs, sizeof(GSource));
	g_source_attach(source, maincontext);
	maincontext = NULL;

	g_object_set(G_OBJECT (pipeline), "volume", volume, NULL);

	g_main_loop_run (loop);

  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (GST_OBJECT (pipeline));

	g_source_unref(source);
	// g_source_remove (bus_watch_id);
	g_main_loop_unref (loop);
}

void media_init() {
  if (MEDIA_IDLE == mediaStatue) {
    volume = VOLUME_MAX / 2;
		mute = false;
    int ret = pthread_create(&mediaThredID, NULL, (void *)media_thread, NULL);
    if (ret != 0) {
      DUER_LOGE ("Create media pthread error!");
      exit(1);
    }
		mediaStatue = MEDIA_PLAY_STOP;
  } else {
    DUER_LOGE ("Error: Media init fail! mediaStatue:%d", mediaStatue);
  }
}

void media_uninit() {
	if (MEDIA_IDLE != mediaStatue) {
		mediaStatue = MEDIA_IDLE;
		pthread_join(mediaThredID, NULL);
		if (url) {
			free(url);
			url = NULL;
		}
	} else {
		DUER_LOGE ("Error: Media uninit fail! mediaStatue:%d", mediaStatue);
	}
}

void media_Play_Start(const char* m_url, t_PalyType pType) {
	DUER_LOGI ("\n>>>media_Play_Start\n");
  if (MEDIA_PLAY_STOP == mediaStatue) {
		if (url) {
			free(url);
			url = NULL;
		}
    url = (char*)malloc(strlen(m_url) + 1);
    strcpy(url, m_url);
		play_type = pType;
		mediaStatue = MEDIA_PLAY_START;
  } else {
    DUER_LOGE ("Error: Media Starting! mediaStatue:%d", mediaStatue);
  }
}

void media_Play_Pause() {
	if (MEDIA_PLAY_START == mediaStatue) {
		mediaStatue = MEDIA_PLAY_PAUSE;
	} else if (MEDIA_PLAY_PAUSE == mediaStatue) {
		mediaStatue = MEDIA_PLAY_START;
	} else {
		DUER_LOGE ("Error: Media Play Pause Fail! mediaStatue:%d", mediaStatue);
	}
}

void media_Play_Seek(const char* m_url, int offset, t_PalyType pType) {
	DUER_LOGI ("\n>>>media_Play_Start\n");
	if (MEDIA_PLAY_STOP == mediaStatue) {
		if (url) {
			free(url);
			url = NULL;
		}
		url = (char*)malloc(strlen(m_url) + 1);
		strcpy(url, m_url);
		play_type = pType;
		s_seek = offset;
	} else {
		DUER_LOGE ("Error: Media Starting! mediaStatue:%d", mediaStatue);
	}
}

void media_Play_Stop() {
	if ((MEDIA_PLAY_START == mediaStatue)
		|| (MEDIA_PLAY_PAUSE == mediaStatue)) {
		mediaStatue = MEDIA_PLAY_STOP;
	} else {
		DUER_LOGE ("Error: Media Play Stop Fail! mediaStatue:%d", mediaStatue);
	}
}

void media_Volume_Change(int m_volume) {
	if (mute) {
		return;
	}
  double tmp = m_volume / 10.0 + volume;

  if (tmp < VOLUME_MIX) {
  	tmp = 0.0;
  } else if (tmp > VOLUME_MAX) {
  	tmp = VOLUME_MAX;
  }
  volume = tmp;
	DUER_LOGI ("\nvolume : %.1f\n", volume);
}

void media_Set_Volume(int m_volume) {
	if (mute) {
		return;
	}
  volume = m_volume / 10.0;
}

int media_Get_Volume() {
  return (int)(volume * 10);
}

void media_Set_Mute(bool m_mute) {
	static double m_volume = 0.0;
  mute = m_mute;
	if (m_mute) {
		if ((m_volume < VOLUME_MIX)
			|| (m_volume > (-VOLUME_MIX))) {
			m_volume = volume;
			volume = 0.0;
		}
	} else {
		if ((volume < VOLUME_MIX)
			|| (volume > (-VOLUME_MIX))) {
			volume = m_volume;
			m_volume = 0.0;
		}
	}
}

bool media_Get_Mute() {
	return mute;
}

int media_Get_Position() {
	return pos / 1000000;
}

void media_Get_State() {
	DUER_LOGI ("\nmediaStatue : %d\n", mediaStatue);
}
