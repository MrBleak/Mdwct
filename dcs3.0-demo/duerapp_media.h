// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_media.h
 * Auth:
 * Desc: Duer Configuration.
 */

#ifndef BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_MEDIA_H
#define BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_MEDIA_H

#include <gst/gst.h>

#include "duerapp_config.h"

typedef enum{
    PLAY_IDLE,
    PLAY_SPEAK,
    PLAY_AUDIO
}duer_play_type_t;

typedef enum{
	MEDIA_IDLE,
	MEDIA_PLAY_START,
	MEDIA_PLAY_STOP
}duer_media_statue_t;

typedef struct{
	double vol;
	duer_play_type_t type;
	GstElement *pipeline;
	GMainLoop *loop;
	int seek;
	bool mute;
}duer_media_params_t;

int duer_media_init();
int duer_media_uninit();

void duer_media_play_start(const char* url, duer_play_type_t play_type);
void duer_media_play_seek(const char* url, int offset, duer_play_type_t play_type);
void duer_media_play_stop();

void duer_media_volume_change(int volume);
void duer_media_set_volume(int volume);
int duer_media_get_volume();
void duer_media_set_mute(bool mute);
bool duer_media_get_mute();
int duer_media_get_position();

#endif // BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_MEDIA_H
