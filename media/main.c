#include <stdio.h>
//#include <malloc.h>
#include <unistd.h>

#include "media.h"

//#define URL_LEN_MAX (2048)

int main()
{
  char* url = "http://audio.xmcdn.com/group19/M0A/6C/BB/wKgJK1f78ayTh52oAGEtCcDT_UY493.m4a";

  media_init();
  media_Play_Start(url);
  while(1);
  {
    sleep(1);
    printf("volume:%.2f\r", media_Get_Volume());
  }
  sleep(3);
  media_Play_Pause();
  sleep(3);
  media_Play_Pause();
  sleep(3);
  media_Volume_Change(-2.0);
  sleep(3);
  media_Volume_Change(4.0);
  sleep(3);
  media_Set_Volume(2.0);
  sleep(3);
  media_Set_Volume(8.0);
  sleep(3);
  media_Mute(1);
  sleep(3);

  printf("volume:%.2f\n", media_Get_Volume());
  sleep(3);

  media_Mute(0);
  sleep(3);
  media_Play_Stop();
  sleep(3);

  return 0;
}
