// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_recorder.c
 * Auth:
 * Desc: Duer Configuration.
 */

#include <alsa/asoundlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "duerapp_recorder.h"
#include "duerapp_config.h"
#include "lightduer_voice.h"

#define ALSA_PCM_NEW_HW_PARAMS_API

typedef enum{
	RECORDER_UNINIT,
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

static duer_rec_state_t s_duer_rec_state = RECORDER_UNINIT;
static pthread_t s_rec_thredID;
static duer_rec_config_t* s_index = NULL;

void duer_open_alsa_pcm()
{
	int ret = (snd_pcm_open(&(s_index->handle), "default", SND_PCM_STREAM_CAPTURE, 0));
	if (ret < 0) {
		DUER_LOGE("unable to open pcm device: %s", snd_strerror(ret));
		exit(1);
	}
}

void duer_set_pcm_params()
{
	int ret;
	s_index->frames = 32;
	s_index->val = 16000;
	snd_pcm_hw_params_alloca(&(s_index->params));
	snd_pcm_hw_params_any(s_index->handle, s_index->params);
	snd_pcm_hw_params_set_access(s_index->handle, s_index->params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(s_index->handle, s_index->params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels(s_index->handle, s_index->params, 1);
	snd_pcm_hw_params_set_rate_near(s_index->handle, s_index->params,  &(s_index->val), &(s_index->dir));
	snd_pcm_hw_params_set_period_size_near(s_index->handle, s_index->params, &(s_index->frames), &(s_index->dir));

	ret = snd_pcm_hw_params(s_index->handle, s_index->params);
	if (ret < 0) {
		DUER_LOGE("unable to set hw parameters: %s", snd_strerror(ret));
		exit(1);
	}
	snd_pcm_hw_params_get_period_size(s_index->params,  &(s_index->frames), &(s_index->dir));
	s_index->size = s_index->frames * 2;
}

void recorder_thread()
{
	while (1) {
		usleep(1);
		if (RECORDER_START == s_duer_rec_state) {
			printf ("1");
			duer_open_alsa_pcm();
			duer_set_pcm_params();
			char *buffer = (char *)malloc (s_index->size);
			if (!buffer) {
				DUER_LOGE("malloc rec buffer failde!");
			}
			FILE *fp = fopen("a.pcm", "w");
			while (RECORDER_START == s_duer_rec_state) {
				int ret = snd_pcm_readi(s_index->handle, buffer, s_index->frames);

				if (ret == -EPIPE) {
					/* EPIPE means overrun */
					DUER_LOGE("overrun occurred");
					snd_pcm_prepare(s_index->handle);
				}	else if (ret < 0) {
					DUER_LOGE("error from read: %s", snd_strerror(ret));
				}	else if (ret != (int)s_index->frames) {
					DUER_LOGE("short read, read %d frames", ret);
				}
				fwrite(buffer,1, s_index->size,fp);
				duer_voice_send(buffer, s_index->size);
			}
			fclose(fp);
			if (buffer) {
				free(buffer);
				buffer = NULL;
			}
		} else if (RECORDER_STOP == s_duer_rec_state) {
				snd_pcm_drain(s_index->handle);
				snd_pcm_close(s_index->handle);
				s_duer_rec_state = RECORDER_IDEL;
		} else if (RECORDER_UNINIT == s_duer_rec_state) {
			break;
		} else if (RECORDER_IDEL == s_duer_rec_state) {
			continue;
		}
	}
}

void duer_record_init()
{
	DUER_LOGI ("duer_record_init");
	s_index = (duer_rec_config_t*)malloc (sizeof(duer_rec_config_t));

	if ((s_index) && (RECORDER_UNINIT == s_duer_rec_state)) {
		int ret = pthread_create(&s_rec_thredID, NULL, (void *)recorder_thread, NULL);
		if(ret != 0) {
			DUER_LOGE("Create recorder pthread error!");
			exit(1);
		}
		s_duer_rec_state = RECORDER_IDEL;
	}	else {
		DUER_LOGE("Error: Recorder Starting!");
	}
}

void duer_record_uninit()
{
	if (RECORDER_UNINIT != s_duer_rec_state) {
		s_duer_rec_state = RECORDER_UNINIT;
		pthread_join(s_rec_thredID,NULL);
		free(s_index);
		s_index = NULL;
	} else {
		DUER_LOGE("Error: Recorder uninit failed!");
	}
}

void duer_recorder_start()
{
	if (RECORDER_IDEL == s_duer_rec_state) {
		s_duer_rec_state = RECORDER_START;
	}	else {
		DUER_LOGE("Error: Recorder don't Start!");
	}
}

void duer_recorder_stop()
{
	DUER_LOGI ("duer_recorder_stop");
	if (RECORDER_START == s_duer_rec_state)	{
		s_duer_rec_state = RECORDER_STOP;
	}	else {
		DUER_LOGE("Error: Recorder don't Stop!");
	}
}
