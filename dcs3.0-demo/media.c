#include <string.h>
#include <gst/gst.h>
#include <assert.h>
#include <malloc.h>

#include "media.h"

#define VOLUME_MAX (10.0)
#define VOLUME_MIX (0.000001)

typedef enum{
		MEDIA_IDLE,
		MEDIA_PLAY_START,
	  MEDIA_PLAY_PAUSE,
		MEDIA_PLAY_STOP
}t_Media_Statue;

typedef struct{
		bool volumChange;
		bool playStateChange;
}t_Change;

volatile char* url = NULL;
volatile t_Media_Statue mediaStatue = MEDIA_IDLE;
volatile double volume;
static pthread_t mediaThredID;
static bool mute;
static t_Change change = {
		.volumC = FALSE,
		.playSC = FALSE
};
GstElement* pipeline = NULL;
GMainLoop* loop = NULL;

static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data)
{
	  GMainLoop *m_loop = (GMainLoop *) data;

	  switch (GST_MESSAGE_TYPE (msg))
		{
				case GST_MESSAGE_EOS :
						{
								printf ("Info:media Play End\n");
					      g_main_loop_quit (m_loop);
						}
						break;
				default:
						break;
		}
	  return TRUE;
}

gboolean source_prepare_cb(GSource * source, gint * timeout)
{
    *timeout = 200;
    return FALSE;
}

gboolean source_check_cb(GSource * source)
{
		static double m_volume = volume;
		static t_Media_Statue m_mediaState = mediaStatue;
		if ((m_volume - volume) > VOLUME_MIX) || ((m_volume - volume) < (-VOLUME_MIX))
		{
				m_volume = volume;
				change.volumC = TRUE;
		}
		if (m_mediaState != mediaStatue)
		{
				m_mediaState = mediaStatue;
				change.playSC = TRUE;
		}
    return (change.volumC || change.playSC);
}

gboolean source_dispatch_cb(GSource * source, GSourceFunc callback, gpointer data)
{
		if (change.volumC)
		{
				g_object_set(G_OBJECT (pipeline), "volume", volume, NULL);
		}
		if (change.playSC)
		{
				if (MEDIA_PLAY_START == mediaStatue)
				{
						gst_element_set_state (pipeline, GST_STATE_PLAYING);
				}
				else (MEDIA_PLAY_PAUSE == mediaStatue)
				{
						gst_element_set_state (pipeline, GST_STATE_PAUSED);
				}
				else (MEDIA_PLAY_STOP == mediaStatue)
				{
						gst_element_set_state (pipeline, GST_STATE_READY);
						g_main_loop_quit (loop);
				}
		}
	  return TRUE;
}

#if 0
void source_finalize_cb(GSource * source)
{

}
#endif

void media_thread()
{
	  gst_init(NULL,NULL);
	  pipeline = gst_element_factory_make("playbin", "play");
	  assert(pipeline!=NULL);
		GstBus* bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
		loop = g_main_loop_new(NULL,FALSE);
		guint bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
		gst_object_unref (bus);

		GMainContext * maincontext;
		GSource * source;
		GSourceFuncs sourcefuncs = {
				sourcefuncs.prepare = source_prepare_cb,
				sourcefuncs.check = source_check_cb,
				sourcefuncs.dispatch = source_dispatch_cb,
				// sourcefuncs.finalize = source_finalize_cb
		};

		maincontext = g_main_loop_get_context(loop);
		source = g_source_new(&sourcefuncs, sizeof(GSource));
		g_source_attach(source, maincontext);

	  while (MEDIA_IDLE != mediaStatue)
	  {
		    if (url) && (MEDIA_PLAY_START != mediaStatue)
		    {
						g_object_set(G_OBJECT (pipeline), "volume", volume, NULL);
			      g_object_set (G_OBJECT (pipeline), "uri", url, NULL);
						gst_element_set_state (pipeline, GST_STATE_PLAYING);
						free(url);
						url = NULL;
						g_main_loop_run (loop);
		    }
	  }
	  gst_element_set_state (pipeline, GST_STATE_NULL);
	  gst_object_unref (GST_OBJECT (pipeline));
		g_source_remove (bus_watch_id);
		g_main_loop_unref (loop);
}

void media_init()
{
	  if (MEDIA_IDLE == mediaStatue)
	  {
		    volume = VOLUME_MAX / 2;
		    int ret = pthread_create(&mediaThredID, NULL, (void *)media_thread, NULL);
		    if (ret!=0)
		    {
			      printf("Create media pthread error!\n");
			      exit(1);
		    }
				mediaStatue = MEDIA_PLAY_STOP;
	  }
	  else
	  {
		    printf("Error: Media init fail! mediaStatue:%d\n", mediaStatue);
	  }
}

void media_uninit()
{
		if (MEDIA_IDLE != mediaStatue)
		{
				mediaStatue = MEDIA_IDLE;
				pthread_join();
		}
		else
		{
				printf("Error: Media uninit fail! mediaStatue:%d\n", mediaStatue);
		}
}

void media_Play_Start(char* m_url)
{
	  if (MEDIA_PLAY_STOP == mediaStatue)
	  {
				if (url)
				{
					free(url);
					url = NULL;
				}
		    url = (char*)malloc(strlen(m_url) + 1);
		    strcpy(url, m_url);
				mediaStatue = MEDIA_PLAY_START;
	  }
	  else
	  {
		    printf("Error: Media Starting! mediaStatue:%d\n", mediaStatue);
	  }
}

void media_Play_Pause()
{
		if (MEDIA_PLAY_START == mediaStatue)
		{
				mediaStatue = MEDIA_PLAY_PAUSE;
		}
		else if (MEDIA_PLAY_PAUSE == mediaStatue)
		{
				mediaStatue = MEDIA_PLAY_START;
		}
		else
		{
				printf("Error: Media Play Pause Fail! mediaStatue:%d\n", mediaStatue);
		}
}

void media_Play_Seek()
{

}
void media_Play_Stop()
{
		if ((MEDIA_PLAY_START == mediaStatue) || (MEDIA_PLAY_PAUSE == mediaStatue))
		{
				mediaStatue = MEDIA_PLAY_STOP;
		}
		else
		{
				printf("Error: Media Play Stop Fail! mediaStatue:%d\n", mediaStatue);
		}
}

void media_Volume_Change(double m_volume)
{
		if (mute)
		{
				return;
		}
	  double tmp = m_volume + volume;

	  if (tmp < VOLUME_MIX)
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
		if (mute)
		{
				return;
		}
	  volume = m_volume;
}

double media_Get_Volume()
{
  	return volume;
}

void media_Mute(bool m_mute)
{
		static double m_volume = 0.0;
	  mute = m_mute;
		if (m_mute)
		{
				if ((m_volume < VOLUME_MIX) || (m_volume > (-VOLUME_MIX)))
				{
						m_volume = volume;
						volume = 0.0;
				}
		}
		else
		{
				if ((volume < VOLUME_MIX) || (volume > (-VOLUME_MIX)))
				{
						volume = m_volume;
						m_volume = 0.0;
				}
		}
}
