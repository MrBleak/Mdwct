#include <glib.h>

#include <gst/gst.h>

#include <assert.h>

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
  if (argc != 2) {
    g_printerr ("Usage: %s <uri>\n", argv[0]);
    return -1;
  }

  gst_init(&argc,&argv);
  /* Set up the pipeline */
  GstElement* pipeline = gst_element_factory_make("playbin", "play");
  assert(pipeline!=NULL);
  /* we set the input filename to the source element */
  g_object_set (G_OBJECT (pipeline), "uri", argv[1], NULL);
  /* we add a message handler */
  //GstBus* bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  GMainLoop* loop = g_main_loop_new(NULL,FALSE);
  //guint bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
  //gst_object_unref (bus);
  /* Set the pipeline to "playing" state*/
  g_print ("Now playing: %s\n", argv[1]);
  gst_element_set_state (pipeline, GST_STATE_PLAYING);
  /* Iterate */
  g_timeout_add (200, (GSourceFunc) cb_print_position, pipeline);
  g_main_loop_run (loop);
  /* Out of the main loop, clean up nicely */
  g_print ("Returned, stopping playback\n");
  gst_element_set_state (pipeline, GST_STATE_NULL);
  g_print ("Deleting pipeline\n");
  gst_object_unref (GST_OBJECT (pipeline));
  //g_source_remove (bus_watch_id);
  g_main_loop_unref (loop);

  return 0;
}
