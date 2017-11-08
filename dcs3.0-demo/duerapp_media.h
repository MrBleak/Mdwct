// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_media.h
 * Auth:
 * Desc: Duer Configuration.
 */

#ifndef BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_MEDIA_H
#define BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_MEDIA_H

#include "duerapp_config.h"

void media_init();
void media_uninit();

void media_Play_Start(const char* m_url, t_PalyType pType);
void media_Play_Pause();
void media_Play_Seek(const char* m_url, int offset, t_PalyType pType);
void media_Play_Stop();

void media_Volume_Change(int m_volume);
void media_Set_Volume(int m_volume);
int media_Get_Volume();
void media_Set_Mute(bool mute);
bool media_Get_Mute();
int media_Get_Position();
void media_Get_State();

// int get_Play_

#endif // BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_MEDIA_H
