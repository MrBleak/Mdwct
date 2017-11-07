// Copyright (2017) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_kbd_listener.h
 * Auth:
 * Desc: Duer Configuration.
 */

#ifndef BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_KBD_LISTENER_H
#define BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_KBD_LISTENER_H

typedef enum{
	PLAY_PAUSE = 'w',
	RECORD_START = ' ',
	PLAY_STOP = 's'
	PRVIOUS_SONG = 'a',
	NEXT_SONG = 'd',
	VOLUME_INCR = '-',
	VOLUME_DECR = '=',
	RECONNTECT_CLOUD = 'e',
	QUIT = 'q',
}t_KBD_Event;

int scanKeyboard(void);

#endif // BAIDU_DUER_LIBDUER_DEVICE_EXAMPLES_DCS3_LINUX_DUERAPP_KBD_LISTENER_H
