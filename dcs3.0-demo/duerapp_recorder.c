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

#define ALSA_PCM_NEW_HW_PARAMS_API

typedef void (*Recodrd_Listen_Func)(char* data, int size);

typedef enum{
	RECORDER_IDEL,
	RECORDER_START,
	RECORDER_STOP
}t_Recorder_Statue;

typedef struct{
	snd_pcm_t* handle;
	snd_pcm_hw_params_t* params;
	int dir;
	unsigned int val;
	snd_pcm_uframes_t frames;
	int size;
}t_Param;

static Recodrd_Listen_Func* send_data = NULL;
volatile t_Recorder_Statue recordStatue = RECORDER_IDEL;
static pthread_t recordThredID;
static t_Param* arg = NULL;

void recorder_thread(t_Param* _arg)
{

	snd_pcm_hw_params_get_period_size(_arg->params,  &(_arg->frames), &(_arg->dir));

	_arg->size = _arg->frames * 2;
	char *buffer = (char *)malloc(_arg->size);
	while (RECORDER_START == recordStatue)
	{
		int rc = snd_pcm_readi(_arg->handle, buffer, _arg->frames);

		if (rc == -EPIPE)
		{
			/* EPIPE means overrun */
			DUER_LOGE("overrun occurred");
			snd_pcm_prepare(_arg->handle);
		}
		else if (rc < 0)
		{
			DUER_LOGE("error from read: %s", snd_strerror(rc));
		}
		else if (rc != (int)_arg->frames)
		{
			DUER_LOGE("short read, read %d frames", rc);
		}
		(*send_data)(buffer, _arg->size);
		if (RECORDER_STOP == recordStatue)
		{
			recordStatue = RECORDER_IDEL;
			free(buffer);
			buffer = NULL;
		}
	}
	if (NULL != buffer)
	{
		recordStatue = RECORDER_IDEL;
		free(buffer);
		buffer = NULL;
	}
}

void open_alsa_pcm(t_Param* _arg)
{
	int rc = (snd_pcm_open(&(_arg->handle), "default", SND_PCM_STREAM_CAPTURE, 0));
	if (rc < 0)
	{
		DUER_LOGE("unable to open pcm device: %s", snd_strerror(rc));
		exit(1);
	}
}

void set_pcm_params(t_Param* _arg)
{
	int rc;

	snd_pcm_hw_params_alloca(&(_arg->params));
	snd_pcm_hw_params_any(_arg->handle, _arg->params);
	snd_pcm_hw_params_set_access(_arg->handle, _arg->params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(_arg->handle, _arg->params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels(_arg->handle, _arg->params, 1);
	snd_pcm_hw_params_set_rate_near(_arg->handle, _arg->params,  &(_arg->val), &(_arg->dir));
	snd_pcm_hw_params_set_period_size_near(_arg->handle, _arg->params, &(_arg->frames), &(_arg->dir));

	rc = snd_pcm_hw_params(_arg->handle, _arg->params);
	if (rc < 0)
	{
		DUER_LOGE("unable to set hw parameters: %s", snd_strerror(rc));
		exit(1);
	}
}

void recorder_Start()
{
	arg = (t_Param*)malloc(sizeof(t_Param));
	arg->frames = 32;
	arg->val = 16000;
	if (RECORDER_IDEL == recordStatue)
	{
		open_alsa_pcm(arg);
		set_pcm_params(arg);

		int ret = pthread_create(&recordThredID, NULL, (void *)recorder_thread, arg);
		if(ret!=0)
		{
			DUER_LOGE("Create recorder pthread error!");
			exit(1);
		}
		recordStatue = RECORDER_START;
	}
	else
	{
		DUER_LOGE("Error: Recorder Starting!");
	}
}

void recorder_Stop()
{
	if (recordStatue == recordStatue)
	{
		recordStatue = RECORDER_STOP;
		pthread_join(recordThredID,NULL);
		snd_pcm_drain(arg->handle);
		snd_pcm_close(arg->handle);
	}
	else
	{
		DUER_LOGE("Error: Recorder don't Starting!");
	}
}

void set_Recorder_Listener(Recodrd_Listen_Func func)
{
	if (NULL == send_data)
	{
		send_data = (Recodrd_Listen_Func*) malloc(sizeof(Recodrd_Listen_Func));
	}
	if (NULL == func)
	{
		free(send_data);
	}
	*send_data = func;
}
