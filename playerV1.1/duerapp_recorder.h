// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_recorder.h
 * Auth:
 * Desc: Duer Configuration.
 */

#ifndef BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_RECORDER_H
#define BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_RECORDER_H

#include <alsa/asoundlib.h>

typedef enum{
	RECORDER_IDEL,
	RECORDER_START,
	RECORDER_STOP
}duer_rec_state_t;

typedef struct{
	int dir;
	int size;
	unsigned int val;
	snd_pcm_t *handle;
	snd_pcm_uframes_t frames;
	snd_pcm_hw_params_t *params;
}duer_rec_config_t;

int duer_recorder_start();
int duer_recorder_stop();

#endif // BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_RECORDER_H
