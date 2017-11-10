// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_recorder.c
 * Auth:
 * Desc: Duer Configuration.
 */

#include <alsa/asoundlib.h>

#include "duerapp_recorder.h"
#include "duerapp_config.h"

#define ALSA_PCM_NEW_HW_PARAMS_API

static duer_rec_state_t s_duer_rec_state = RECORDER_IDEL;
static pthread_t s_rec_thredID;
static duer_rec_config_t *s_index = NULL;

static void recorder_thread()
{

	snd_pcm_hw_params_get_period_size(s_index->params,
			&(s_index->frames),
			&(s_index->dir));

	s_index->size = s_index->frames * 2;
	char *buffer = (char*)malloc(s_index->size);
	while (RECORDER_START == s_duer_rec_state)
	{
		int ret = snd_pcm_readi(s_index->handle, buffer, s_index->frames);

		if (ret == -EPIPE)
		{
			DUER_LOGE("overrun occurred");
			snd_pcm_prepare(s_index->handle);
		}
		else if (ret < 0)
		{
			DUER_LOGE("error from read: %s", snd_strerror(ret));
		}
		else if (ret != (int)s_index->frames)
		{
			DUER_LOGE("short read, read %d frames", ret);
		}

		/* (*send_data)(buffer, s_index->size); */
		if (RECORDER_STOP == s_duer_rec_state)
		{
			s_duer_rec_state = RECORDER_IDEL;
			free(buffer);
			buffer = NULL;
		}
	}
	if (NULL != buffer)
	{
		s_duer_rec_state = RECORDER_IDEL;
		free(buffer);
		buffer = NULL;
	}
}

static void duer_open_alsa_pcm()
{
	int ret = (snd_pcm_open(&(s_index->handle), "default", SND_PCM_STREAM_CAPTURE, 0));
	if (ret < 0)
	{
		DUER_LOGE("unable to open pcm device: %s", snd_strerror(ret));
		exit(1);
	}
}

static void duer_set_pcm_params()
{
	snd_pcm_hw_params_alloca(&(s_index->params));
	snd_pcm_hw_params_any(s_index->handle, s_index->params);
	snd_pcm_hw_params_set_access(s_index->handle, s_index->params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(s_index->handle, s_index->params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels(s_index->handle, s_index->params, 1);
	snd_pcm_hw_params_set_rate_near(s_index->handle, s_index->params,
			&(s_index->val), &(s_index->dir));
	snd_pcm_hw_params_set_period_size_near(s_index->handle, s_index->params,
			&(s_index->frames), &(s_index->dir));

	int ret = snd_pcm_hw_params(s_index->handle, s_index->params);
	if (ret < 0)
	{
		DUER_LOGE("unable to set hw parameters: %s", snd_strerror(ret));
		exit(1);
	}
}

void duer_recorder_start()
{
	s_index = (duer_rec_config_t*)malloc(sizeof(duer_rec_config_t));
	s_index->frames = 32;
	s_index->val = 16000;
	if (RECORDER_IDEL == s_duer_rec_state)
	{
		open_alsa_pcm();
		set_pcm_params();

		int ret = pthread_create(&s_rec_thredID, NULL, (void *)recorder_thread, NULL);
		if(ret!=0)
		{
			DUER_LOGE("Create recorder pthread error!");
			exit(1);
		}
		s_duer_rec_state = RECORDER_START;
	}
	else
	{
		DUER_LOGE("Error: Recorder Starting!");
	}
}

void duer_recorder_stop()
{
	if (RECORDER_START == s_duer_rec_state)
	{
		s_duer_rec_state = RECORDER_STOP;
		pthread_join(s_rec_thredID,NULL);
		snd_pcm_drain(s_index->handle);
		snd_pcm_close(s_index->handle);
	}
	else
	{
		DUER_LOGE("Error: Recorder don't Starting!");
	}
}
