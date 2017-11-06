#include "recorder.h"
#include <unistd.h>
#include <stdio.h>

FILE *fp = NULL;

void save_data(char* data, int size)
{
	if (!fp)
	{
		return;
	}
	int rc = fwrite(data, 1, size, fp);
	if (rc != size)
	{
		fprintf(stderr, "short write: wrote %d bytes/n", rc);
	}
}

int main()
{
	if((fp = fopen("sound.pcm","wb")) < 0)
	{
		printf("open sound.pcm fial\n");
	}
	set_Recorder_Listener(save_data);
	recorder_Start();
	sleep(10);
	recorder_Stop();
	fclose(fp);
	fp = NULL;
	return 0;
}
