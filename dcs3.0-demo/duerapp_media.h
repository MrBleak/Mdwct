// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_media.h
 * Auth:
 * Desc: Duer Configuration.
 */

#ifndef BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_MEDIA_H
#define BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_MEDIA_H

#include <gst/gst.h>

void media_init();
void media_uninit();

void media_Play_Start(const char* m_url);
void media_Play_Pause();
void media_Play_Seek(const char* m_url, int offset);
void media_Play_Stop();

void media_Volume_Change(double m_volume);
void media_Set_Volume(double m_volume);
double media_Get_Volume();
void media_Mute(gboolean mute);

#endif // BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_MEDIA_H
