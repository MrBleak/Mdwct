#include <string.h>
#include <gst/gst.h>
#include <assert.h>
#include <malloc.h>

GstElement* pipeline = NULL;

static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data)
{
  GMainLoop *loop = (GMainLoop *) data;

  if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_EOS)
	{
			g_print ("End of stream\n");
      g_main_loop_quit (loop);
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
    return TRUE;
}

gboolean source_dispatch_cb(GSource * source, GSourceFunc callback, gpointer data)
{
	gint64 pos, len;
	GstFormat format = GST_FORMAT_TIME;

	if (gst_element_query_position (pipeline, format, &pos) && gst_element_query_duration (pipeline, format, &len))
	{
			g_print ("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r", GST_TIME_ARGS (pos), GST_TIME_ARGS (len));
	}
  return TRUE;
}
void source_finalize_cb(GSource * source)
{

}

int main(int argc, char *argv[])
{
	if (argc != 2) {
    g_printerr ("Usage: %s <uri>\n", argv[0]);
    return -1;
  }

  gst_init(NULL,NULL);
  pipeline = gst_element_factory_make("playbin", "play");
  assert(pipeline!=NULL);
  g_object_set(G_OBJECT (pipeline), "volume", 5.0, NULL);
	GstBus* bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	GMainLoop* loop = g_main_loop_new(NULL,FALSE);
	guint bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
	gst_object_unref (bus);

	GMainContext * maincontext;
	GSource * source;
	GSourceFuncs sourcefuncs = {
		.prepare = source_prepare_cb,
		.check = source_check_cb,
		.dispatch = source_dispatch_cb,
	};

	maincontext = g_main_loop_get_context(loop);
	source = g_source_new(&sourcefuncs, sizeof(GSource));
	g_source_attach(source, maincontext);


			gst_element_set_state (pipeline, GST_STATE_NULL);
			//g_object_set(G_OBJECT (pipeline), "volume", volume, NULL);
      g_object_set (G_OBJECT (pipeline), "uri", argv[1], NULL);
			gst_element_set_state (pipeline, GST_STATE_PLAYING);
			g_main_loop_run (loop);
      //free(url);
      //url = NULL;

  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (GST_OBJECT (pipeline));
	g_source_remove (bus_watch_id);
	
	return 0;
}
