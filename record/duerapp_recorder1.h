// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_recorder.h
 * Auth:
 * Desc: Duer Configuration.
 */

#ifndef BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_RECORDER_H
#define BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_RECORDER_H

typedef enum{
	RECORDER_IDEL,
	RECORDER_START,
	RECORDER_STOP
}duer_rec_state_t;

typedef struct{
	snd_pcm_t* handle;
	snd_pcm_hw_params_t* params;
	int dir;
	unsigned int val;
	snd_pcm_uframes_t frames;
	int size;
}duer_rec_config_t;

void duer_recorder_start();
void duer_recorder_stop();

#endif // BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_RECORDER_H
