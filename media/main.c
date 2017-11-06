#include <gst/gst.h>
#include <glib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>

static void print_one_tag (const GstTagList * list, const gchar * tag, gpointer user_data)
{
  int i, num;

  num = gst_tag_list_get_tag_size (list, tag);
  for (i = 0; i < num; ++i) {
    const GValue *val;

    /* Note: when looking for specific tags, use the g_tag_list_get_xyz() API,
     * we only use the GValue approach here because it is more generic */
    val = gst_tag_list_get_value_index (list, tag, i);
    #if 0
    if (G_VALUE_HOLDS_STRING (val))
    {
      g_print ("\t%20s : %s\n", tag, g_value_get_string (val));
    }
    else if (G_VALUE_HOLDS_UINT (val))
    {
      g_print ("\t%20s : %u\n", tag, g_value_get_uint (val));
    }
    else if (G_VALUE_HOLDS_DOUBLE (val))
    {
      g_print ("\t%20s : %g\n", tag, g_value_get_double (val));
    }
    else if (G_VALUE_HOLDS_BOOLEAN (val))
    {
      g_print ("\t%20s : %s\n", tag, (g_value_get_boolean (val)) ? "true" : "false");
    }
    else if (GST_VALUE_HOLDS_BUFFER (val))
    {
      // g_print ("\t%20s : buffer of size %u\n", tag, GST_BUFFER_SIZE (gst_value_get_buffer (val)));
    }
    else if (GST_VALUE_HOLDS_DATE (val))
    {
      // g_print ("\t%20s : date (year=%u,...)\n", tag, g_date_get_year (gst_value_get_date (val)));
    }
    else
    {
      // g_print ("\t%20s : tag of type '%s'\n", tag, G_VALUE_TYPE_NAME (val));
    }
    #endif
  }
}

static gboolean buffering = FALSE;

static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data)
{
  GMainLoop *loop = (GMainLoop *) data;

  switch (GST_MESSAGE_TYPE (msg)) {

    case GST_MESSAGE_EOS:
      g_print ("End of stream\n");
      g_main_loop_quit (loop);
      break;

    case GST_MESSAGE_ERROR: {
      gchar  *debug;
      GError *error;

      gst_message_parse_error (msg, &error, &debug);
      g_free (debug);

      g_printerr ("Error: %s\n", error->message);
      g_error_free (error);

      g_main_loop_quit (loop);
      break;
    }
    case GST_MESSAGE_TAG:
        {
            GstTagList *tags = NULL;
            gst_message_parse_tag (msg, &tags);
            g_print ("Got tags from element %s:\n", GST_OBJECT_NAME (GST_MESSAGE_SRC(msg)));
                gst_tag_list_foreach (tags, print_one_tag, NULL);
        }
        break;
    case GST_MESSAGE_NEW_CLOCK:
        g_print ("Get NEW_CLOCK MESSAGE from %s \n", GST_OBJECT_NAME (GST_MESSAGE_SRC(msg)));
        break;
    case GST_MESSAGE_BUFFERING:
        {
            gint percent;

            gst_message_parse_buffering (msg, &percent);
            g_print("buffering %d ...\r",percent);
        }
      break;
    default:
      break;
  }

  return TRUE;
}


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

int main (int argc, char *argv[])
{
  /* Check input arguments */
  #if 0
  if (argc != 2) {
    g_printerr ("Usage: %s <uri>\n", argv[0]);
    return -1;
  }
  #endif
  char* url = "http://yinyueshiting.baidu.com/data2/music/bcd15f1993fa6c5e23b0ada671e6adfb/541990286/541990286.mp3?xcode=38ea2ab29bac2ffec85f964b2abbb8a2";
  char* url2 = (char*)malloc(strlen(url) + 1);
  strcpy(url2, url);
  gst_init(NULL,NULL);
  /* Set up the pipeline */
  GstElement* pipeline = gst_element_factory_make("playbin", "play");
  // GstElement* audiosink = gst_element_factory_make("alsasink","audiosink");
  assert(pipeline!=NULL);
  // assert((pipeline!=NULL) || (audiosink!=NULL));
  /* we set the input filename to the source element */
  g_object_set(G_OBJECT (pipeline), "volume", 5.0, NULL);
  // g_object_set(G_OBJECT (pipeline), "audio-sink", audiosink, "volume",m_volume, NULL);
  g_object_set (G_OBJECT (pipeline), "uri", url2, NULL);
  free(url2);
  url2 = NULL;
  /* we add a message handler */
  //GstBus* bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  //GMainLoop* loop = g_main_loop_new(NULL,FALSE);
  //guint bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
  //gst_object_unref (bus);
  /* Set the pipeline to "playing" state*/
  //g_print ("Now playing: %s\n", argv[1]);
  gst_element_set_state (pipeline, GST_STATE_PLAYING);
  //g_object_set(G_OBJECT (pipeline), "volume", 5.0, NULL);
  /* Iterate */
  //g_timeout_add (200, (GSourceFunc) cb_print_position, pipeline);
  //g_main_loop_run (loop);
  /* Out of the main loop, clean up nicely */
  //g_print ("Returned, stopping playback\n");
  sleep(10);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  while(1);
  //g_print ("Deleting pipeline\n");
  gst_object_unref (GST_OBJECT (pipeline));
  //g_source_remove (bus_watch_id);
  //g_main_loop_unref (loop);

  return 0;
}
