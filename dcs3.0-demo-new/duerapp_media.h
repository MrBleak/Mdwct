// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_media.h
 * Auth:
 * Desc: Duer Configuration.
 */

#ifndef BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_MEDIA_H
#define BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_MEDIA_H

#include "duerapp_config.h"

void duer_media_init();
void duer_media_uninit();

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
