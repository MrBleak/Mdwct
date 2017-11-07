// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_recorder.h
 * Auth:
 * Desc: Duer Configuration.
 */

#ifndef BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_RECORDER_H
#define BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_RECORDER_H

typedef void (*Recodrd_Listen_Func)(char* data, int size);

void recorder_Start();
void recorder_Stop();
void set_Recorder_Listener(Recodrd_Listen_Func func);

#endif // BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_RECORDER_H
