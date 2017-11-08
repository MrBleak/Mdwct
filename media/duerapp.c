// Copyright (2017) Baidu Inc. All rights reserved.
/**
 * File: duerapp.c
 * Auth:
 * Desc: Duer Application Main.
 */

#include "config.h"


extern pthread_t kbdThredID;

char* url = NULL;


void play_end() {
	printf("play_end\n");
}
int main(int argc, char* argv[])
{
	/* Check input arguments */
	if (argc != 2) {
		printf ("Usage: %s <url>\n", argv[0]);
		return -1;
	}
	url = (char*)malloc (strlen(argv[1]) + 1);

	media_init();
	strcpy(url, argv[1]);
	set_Play_Stop_Listener(play_end);
	if (-1 == event_queue_init()) {
		DUER_LOGE ("Create envet queue failed!");
		return -1;
	}

	// try conntect baidu cloud


	event_loop();
	printf("event_loop_end\n");

	return 0;
}
