// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_recorder.c
 * Auth:
 * Desc: Duer Configuration.
 */

#include <alsa/asoundlib.h>
#include <pthread.h>
#include <stdio.h>

#include "duerapp_recorder.h"
#include "duerapp_config.h"
#include "lightduer_voice.h"

#define ALSA_PCM_NEW_HW_PARAMS_API

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
}duer_rec_param_t;

static duer_rec_state_t s_duer_rec_state = RECORDER_IDEL;
static pthread_t s_rec_thredID;
static duer_rec_param_t* s_arg = NULL;

void recorder_thread(duer_rec_param_t* data) {
	snd_pcm_hw_params_get_period_size(data->params,  &(data->frames), &(data->dir));

	data->size = data->frames * 2;
	char *buffer = (char *)malloc(data->size);
	while (RECORDER_START == recordStatue) {
		int rc = snd_pcm_readi(data->handle, buffer, data->frames);

		if (rc == -EPIPE) {
			/* EPIPE means overrun */
			DUER_LOGE("overrun occurred");
			snd_pcm_prepare(data->handle);
		}
		else if (rc < 0) {
			DUER_LOGE("error from read: %s", snd_strerror(rc));
		}	else if (rc != (int)data->frames) {
			DUER_LOGE("short read, read %d frames", rc);
		}

		duer_voice_send(buffer, data->size);

		if (RECORDER_STOP == recordStatue) {
			recordStatue = RECORDER_IDEL;
			free(buffer);
			buffer = NULL;
		}
	}
	if (NULL != buffer) {
		recordStatue = RECORDER_IDEL;
		free(buffer);
		buffer = NULL;
	}
}

void open_alsa_pcm(duer_rec_param_t* data) {
	int rc = (snd_pcm_open(&(data->handle), "default", SND_PCM_STREAM_CAPTURE, 0));
	if (rc < 0) {
		DUER_LOGE("unable to open pcm device: %s", snd_strerror(rc));
		exit(1);
	}
}

void set_pcm_params(duer_rec_param_t* data) {
	int rc;

	snd_pcm_hw_params_alloca(&(data->params));
	snd_pcm_hw_params_any(data->handle, data->params);
	snd_pcm_hw_params_set_access(data->handle, data->params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(data->handle, data->params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels(data->handle, data->params, 1);
	snd_pcm_hw_params_set_rate_near(data->handle, data->params,  &(data->val), &(data->dir));
	snd_pcm_hw_params_set_period_size_near(data->handle, data->params, &(data->frames), &(data->dir));

	rc = snd_pcm_hw_params(data->handle, data->params);
	if (rc < 0) {
		DUER_LOGE("unable to set hw parameters: %s", snd_strerror(rc));
		exit(1);
	}
}

void duer_recorder_start() {
	DUER_LOGI ("duer_recorder_start");
	s_arg = (duer_rec_param_t*)malloc (sizeof(duer_rec_param_t));
	s_arg->frames = 32;
	s_arg->val = 16000;
	if (RECORDER_IDEL == recordStatue) {
		open_alsa_pcm(s_arg);
		set_pcm_params(s_arg);

		int ret = pthread_create(&s_rec_thredID, NULL, (void *)recorder_thread, s_arg);
		if(ret != 0) {
			DUER_LOGE("Create recorder pthread error!");
			exit(1);
		}
		recordStatue = RECORDER_START;
	}	else {
		DUER_LOGE("Error: Recorder Starting!");
	}
}

void duer_recorder_stop() {
	DUER_LOGI ("duer_recorder_stop");
	if (recordStatue == recordStatue)	{
		recordStatue = RECORDER_STOP;
		pthread_join(s_rec_thredID,NULL);
		snd_pcm_drain(s_arg->handle);
		snd_pcm_close(s_arg->handle);
	}	else {
		DUER_LOGE("Error: Recorder don't Starting!");
	}
}
