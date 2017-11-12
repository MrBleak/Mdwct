// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_media.c
 * Auth:
 * Desc: Duer Configuration.
 */


#include <assert.h>

#include "duerapp_media.h"
#include "duerapp_config.h"
#include "lightduer_dcs.h"

#define VOLUME_MAX (10.0)
#define VOLUME_MIX (0.000001)
#define VOLUME_INIT (2.0)

static duer_media_statue_t s_media_statue = MEDIA_IDLE;
static duer_media_params_t *s_param;
static pthread_t s_media_thredID;

static gboolean bus_call_back(GstBus *bus, GstMessage *msg, gpointer data)
{
	switch (GST_MESSAGE_TYPE (msg)) {
		case GST_MESSAGE_EOS: {
				s_media_statue = MEDIA_PLAY_STOP;

				gst_element_set_state(s_param->pipeline, GST_STATE_READY);

				if (PLAY_SPEAK == s_param->type) {
					DUER_LOGV ("PLAY_SPEAK : play end");
					duer_dcs_speech_on_finished();
				} else if (PLAY_AUDIO == s_param->type) {
					DUER_LOGV ("PLAY_AUDIO : play end");
					duer_dcs_audio_on_finished();
				}
				s_param->type = PLAY_IDLE;
			}
			break;
		case GST_MESSAGE_ERROR: {
				s_media_statue = MEDIA_PLAY_STOP;

				gst_element_set_state(s_param->pipeline, GST_STATE_READY);

				s_param->type = PLAY_IDLE;
				DUER_LOGE ("duer merdia play error");
			}
			break;
		default:
			break;
	}
}

void media_thread()
{
	gst_init(NULL,NULL);
	s_param->pipeline = gst_element_factory_make("playbin", "play");
	assert(s_param->pipeline != NULL);
	s_param->loop = g_main_loop_new(NULL,FALSE);

	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE (s_param->pipeline));
	guint bus_id = gst_bus_add_watch(bus, bus_call_back, NULL);
	gst_object_unref(bus);
	g_object_set(G_OBJECT (s_param->pipeline), "volume", s_param->vol, NULL);
	gst_element_set_state(s_param->pipeline, GST_STATE_READY);

	g_main_loop_run(s_param->loop);

  	gst_element_set_state(s_param->pipeline, GST_STATE_NULL);

	g_source_remove(bus_id);
	g_main_loop_unref(s_param->loop);
	gst_object_unref(GST_OBJECT (s_param->pipeline));
}

int duer_media_init()
{
	if (MEDIA_IDLE == s_media_statue) {
		s_param = (duer_media_params_t*)malloc(sizeof(duer_media_params_t));
		if (!s_param) {
			DUER_LOGE ("malloc failed!");
			goto ERROR;
		}
		s_param->vol = VOLUME_INIT;
		s_param->type = PLAY_IDLE;
		s_param->pipeline = NULL;
		s_param->loop = NULL;
		s_param->mute = FALSE;
		int ret = pthread_create(&s_media_thredID, NULL, (void *)media_thread, NULL);
		if (ret != 0) {
			DUER_LOGE ("Create media pthread failed!");
			goto ERROR;
		}
		s_media_statue = MEDIA_PLAY_STOP;
	} else {
		DUER_LOGE ("Media init fail! state:%d", s_media_statue);
		goto ERROR;
	}
	return DUER_OK;

ERROR:
	if (s_param) {
		free(s_param);
		s_param = NULL;
	}
	return DUER_ERR_FAILED;
}

int duer_media_uninit()
{
	int ret = DUER_OK;
	if (MEDIA_IDLE != s_media_statue) {
		s_media_statue = MEDIA_IDLE;
		g_main_loop_quit(s_param->loop);
		pthread_join(s_media_thredID, NULL);
		if (s_param) {
			free(s_param);
			s_param = NULL;
		}
	} else {
		ret = DUER_ERR_FAILED;
		DUER_LOGE ("Media uninit fail! state:%d", s_media_statue);
	}
	return ret;
}

void duer_media_play_start(const char* url, duer_play_type_t play_type)
{
	DUER_LOGI ("Media play start : %d : %s", play_type, url);
	if (MEDIA_PLAY_STOP == s_media_statue) {
		g_object_set (G_OBJECT (s_param->pipeline), "uri", url, NULL);

		gst_element_set_state(s_param->pipeline, GST_STATE_PLAYING);

		s_param->type = play_type;
		s_media_statue = MEDIA_PLAY_START;
    } else {
        DUER_LOGE ("Media statue : %d", s_media_statue);
    }
}

void duer_media_play_seek(const char* url,
			                int offset,
			                duer_play_type_t play_type)
{
	DUER_LOGI ("Media play seek : %d : %s", offset, url);
	if (MEDIA_PLAY_STOP == s_media_statue) {
		g_object_set (G_OBJECT (s_param->pipeline), "uri", url, NULL);

		gst_element_set_state(s_param->pipeline, GST_STATE_PLAYING);
		gst_element_seek_simple(s_param->pipeline, 
                                GST_FORMAT_TIME,
					            GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT,
					            GST_MSECOND * s_param->seek);

		s_param->seek = offset;
		s_media_statue = MEDIA_PLAY_START;
    } else {
        DUER_LOGE ("Media statue : %d", s_media_statue);
    }
}

void duer_media_play_stop()
{
	if (MEDIA_PLAY_START == s_media_statue) {

		GstStateChangeReturn gscr = gst_element_set_state(s_param->pipeline, GST_STATE_READY);

		s_param->type = PLAY_IDLE;
		s_media_statue = MEDIA_PLAY_STOP;
	} else {
		DUER_LOGE ("Media statue : %d", s_media_statue);
	}
}

void duer_media_volume_change(int volume)
{
	if (s_param->mute) {
		return;
	}
	s_param->vol += volume / 10.0;

	if (s_param->vol < VOLUME_MIX) {
		s_param->vol = 0.0;
	} else if (s_param->vol > VOLUME_MAX) {
		s_param->vol = VOLUME_MAX;
	}
	g_object_set(G_OBJECT (s_param->pipeline), "volume", s_param->vol, NULL);
	DUER_LOGI ("volume : %.1f", s_param->vol);
}

void duer_media_set_volume(int volume)
{
	if (s_param->mute) {
		return;
	}
	s_param->vol = volume / VOLUME_MAX;
	g_object_set(G_OBJECT (s_param->pipeline), "volume", s_param->vol, NULL);
	DUER_LOGI ("volume : %.1f", s_param->vol);
}

int duer_media_get_volume()
{
    return (int)(s_param->vol * 10);
}

void duer_media_set_mute(bool mute)
{
	s_param->mute = mute;
	if (mute) {
		g_object_set(G_OBJECT (s_param->pipeline), "volume", 0.0, NULL);
	} else {
		g_object_set(G_OBJECT (s_param->pipeline), "volume", s_param->vol, NULL);
	}
	DUER_LOGI ("mute : %d", mute);
}

bool duer_media_get_mute()
{
	return s_param->mute;
}

int duer_media_get_position()
{
	gint64 pos;
	gst_element_query_position (s_param->pipeline, GST_FORMAT_TIME, &pos);
	return pos / GST_MSECOND;
}

