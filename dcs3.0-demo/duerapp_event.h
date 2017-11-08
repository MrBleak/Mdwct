// Copyright (2017) Baidu Inc. All rights reserved.
/**
 * File: event.c
 * Auth:
 * Desc: Duer Application Main.
 */

#ifndef BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_EVENT_H
#define BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_EVENT_H

enum duer_kbd_events{
	PLAY_PAUSE    = 0x7A,
	RECORD_START  = 0x78,
	PLAY_STOP     = 0x63,
	PREVIOUS_SONG = 0x61,
	NEXT_SONG     = 0x64,
	VOLUME_INCR   = 0x77,
	VOLUME_DECR   = 0x73,
	VOLUME_MUTE   = 0x65,
	QUIT          = 0x71,
};

void duer_event_loop();

#endif // BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_EVENT_H
