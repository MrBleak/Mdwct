// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_recorder.c
 * Auth:
 * Desc: Duer Configuration.
 */

#include "duerapp_recorder.h"
#include "duerapp_config.h"
#include "lightduer_voice.h"

#define ALSA_PCM_NEW_HW_PARAMS_API
#define SAMPLE_RATE (16000)
#define FRAMES_INIT (16)
#define FRAMES_SIZE (2) // bytes / sample * channels
#define CHANNEL (1)

static duer_rec_state_t s_duer_rec_state = RECORDER_IDEL;
static pthread_t s_rec_thredID;
static duer_rec_config_t *s_index = NULL;

static void recorder_thread()
{

	snd_pcm_hw_params_get_period_size(s_index->params, &(s_index->frames), &(s_index->dir));

	s_index->size = s_index->frames * FRAMES_SIZE;
	char *buffer = (char*)malloc(s_index->size);
	if (!buffer) {
		DUER_LOGE("malloc buffer failed!\n");
	} else {
		memset(buffer, 0, s_index->size);
	}
	while (RECORDER_START == s_duer_rec_state)
	{
		int ret = snd_pcm_readi(s_index->handle, buffer, s_index->frames);

		if (ret == -EPIPE)
		{
			DUER_LOGE("an overrun occurred!");
			snd_pcm_prepare(s_index->handle);
		}
		else if (ret < 0)
		{
			DUER_LOGE("read: %s", snd_strerror(ret));
		}
		else if (ret != (int)s_index->frames)
		{
			DUER_LOGE("read %d frames!", ret);
		}
		duer_voice_send(buffer, s_index->size);
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
	snd_pcm_drain(s_index->handle);
	snd_pcm_close(s_index->handle);
}

static int duer_open_alsa_pcm()
{
	int ret = DUER_OK;
	int result = (snd_pcm_open(&(s_index->handle), "default", SND_PCM_STREAM_CAPTURE, 0));
	if (result < 0)
	{
		DUER_LOGE("unable to open pcm device: %s", snd_strerror(ret));
		ret = DUER_ERR_FAILED;
	}
    return ret;
}

static int duer_set_pcm_params()
{
	int ret = DUER_OK;
	snd_pcm_hw_params_alloca(&(s_index->params));
	snd_pcm_hw_params_any(s_index->handle, s_index->params);
	snd_pcm_hw_params_set_access(s_index->handle, s_index->params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(s_index->handle, s_index->params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels(s_index->handle, s_index->params, CHANNEL);
	snd_pcm_hw_params_set_rate_near(s_index->handle, s_index->params, &(s_index->val), &(s_index->dir));
	snd_pcm_hw_params_set_period_size_near(s_index->handle, s_index->params, &(s_index->frames), &(s_index->dir));

	int result = snd_pcm_hw_params(s_index->handle, s_index->params);
	if (result < 0)	{
		DUER_LOGE("unable to set hw parameters: %s", snd_strerror(ret));
		ret = DUER_ERR_FAILED;
	}
	return ret;
}

int duer_recorder_start()
{
	s_index = (duer_rec_config_t*)malloc(sizeof(duer_rec_config_t));
	if (!s_index) {
		goto ERROR;
	}
	memset(s_index, 0, sizeof(duer_rec_config_t));
    s_index->frames = FRAMES_INIT;
	s_index->val = SAMPLE_RATE; // pcm sample rate
	if (RECORDER_IDEL == s_duer_rec_state)
	{
		int ret = duer_open_alsa_pcm();
        if (ret != DUER_OK) {
            DUER_LOGE ("open pcm failed");
            goto ERROR;
        }

		ret = duer_set_pcm_params();
        if (ret != DUER_OK) {
            DUER_LOGE ("open pcm failed");
            goto ERROR;
        }

		ret = pthread_create(&s_rec_thredID, NULL, (void*)recorder_thread, NULL);
		if(ret != 0)
		{
			DUER_LOGE("Create recorder pthread error!");
			goto ERROR;
		}
		s_duer_rec_state = RECORDER_START;
	}
	else
	{
		DUER_LOGE("Error: Recorder Starting!");
		goto ERROR;
	}
	return DUER_OK;

ERROR:
	if(s_index) {
		free(s_index);
		s_index = NULL;
	}
	return DUER_ERR_FAILED;
}

int duer_recorder_stop()
{
	int ret = DUER_OK;
	if (RECORDER_START == s_duer_rec_state)
	{
		s_duer_rec_state = RECORDER_STOP;
		pthread_join(s_rec_thredID,NULL);
		if(s_index) {
			free(s_index);
			s_index = NULL;
		}
	}
	else
	{
		ret = DUER_ERR_FAILED;
		DUER_LOGE("Error: Recorder don't Starting!");
	}
	return ret;
}

