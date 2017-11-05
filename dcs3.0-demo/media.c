#include <string.h>
#include <gst/gst.h>
#include <assert.h>
#include <malloc.h>

#include "media.h"

#define VOLUME_MAX (10.0)

typedef struct{
  int i;
}t_Param;

typedef enum{
	MEDIA_IDLE,
	MEDIA_START,
  MEDIA_PAUSE,
	MEDIA_STOP
}t_Media_Statue;

volatile char* url = NULL;
volatile t_Media_Statue mediaStatue = MEDIA_IDLE;
volatile double volume;
static pthread_t mediaThredID;
static t_Param* arg = NULL;
static bool mute;
static GstElement* pipeline = NULL;

static gboolean cb_print_position (GstElement *pipeline)
{
  gint64 pos, len;
  GstFormat format = GST_FORMAT_TIME;

  if (gst_element_query_position (pipeline, format, &pos)
    && gst_element_query_duration (pipeline, format, &len)) {
    g_print ("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
         GST_TIME_ARGS (pos), GST_TIME_ARGS (len));
  }

  /* call me again */
  return TRUE;
}

void gstreamerPlay()
{
  g_object_set (G_OBJECT (pipeline), "uri", url, NULL);
  gst_element_set_state (pipeline, GST_STATE_PLAYING);
}

void media_thread()
{
  gst_init(NULL,NULL);
  pipeline = gst_element_factory_make("playbin", "play");
  assert(pipeline!=NULL);
  g_object_set(G_OBJECT (pipeline), "volume", volume, NULL);
  while (MEDIA_IDLE != mediaStatue)
  {
    if(url)
    {
      gstreamerPlay();
      free(url);
      url = NULL;
    }
  }
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (GST_OBJECT (pipeline));
}

void media_init()
{
  if (MEDIA_IDLE == mediaStatue)
  {
    mediaStatue = MEDIA_STOP;
    volume = VOLUME_MAX / 2;
    int ret = pthread_create(&mediaThredID, NULL, (void *)media_thread, arg);
    if (ret!=0)
    {
      mediaStatue = MEDIA_IDLE;
      printf("Create media pthread error!\n");
      exit(1);
    }
  }
  else
  {
    printf("Error: Media init fail!\n");
  }
}

void media_uninit()
{

}

void media_Play_url(char* m_url)
{
  if (MEDIA_STOP == mediaStatue)
  {
    url = (char*)malloc(strlen(_url) + 1);
    strcpy(url, _url);
    mediaStatue = MEDIA_START;
  }
  else
  {
    printf("Error: Media Starting!\n");
  }
}

void media_Volume_Change(double m_volume)
{
  double tmp = m_volume + volume;

  if (tmp < 0)
  {
    tmp = 0.0;
  }
  else if (tmp > VOLUME_MAX)
  {
    tmp = VOLUME_MAX;
  }
  volume = tmp;
}

void media_Set_Volume(double m_volume)
{
  volume = m_volume;
}

double media_Get_Volume()
{
  return volume;
}

void media_Mute(bool m_mute)
{
  mute = m_mute;
}
void media_Play_Pause()
{

}

void media_Play_Seek()
{

}
void media_Stop()
{

}
http://blog.csdn.net/wzwxiaozheng/article/details/18349945
