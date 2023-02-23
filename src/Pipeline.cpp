#include "Pipeline.hpp"

#include <gst/app/app.h>
#include <cstdio>

Pipeline::Pipeline(int argc, char **argv)
{
    gst_init(&argc, &argv);

    createElements();
    linkElements();
}

Pipeline::~Pipeline()
{
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}

void Pipeline::createElements()
{
    pipeline = gst_pipeline_new("pipeline");
    queue1 = gst_element_factory_make("queue", "queue0");
    queue2 = gst_element_factory_make("queue", "queue1");
    videoconvert = gst_element_factory_make("videoconvert", "videoconvert0");
    autoexposure = gst_element_factory_make("autoexposure", "autoexposure0");
    appsink = gst_element_factory_make("appsink", "videosink0");
    imageFreeze = gst_element_factory_make("freeze", "freeze0");

#ifdef DEBUG_MODE
    videosrc = gst_element_factory_make("videotestsrc", "videosrc0");
#else
    videosrc = gst_element_factory_make("v4l2src", "videosrc0");
    autofocus = gst_element_factory_make("autofocus", "autofocus0");
    barcodereader = gst_element_factory_make("barcodereader", "barcodereader0");
#endif
}

void Pipeline::linkElements()
{
    GstCaps *caps = caps = gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "GRAY8", NULL);
    GstElement *filter = gst_element_factory_make("capsfilter","filter");
#ifdef DEBUG_MODE
    gst_bin_add_many(GST_BIN(pipeline), videosrc,imageFreeze , appsink, NULL);

    if (!videosrc || !appsink || !pipeline || !imageFreeze)
    {
        printf("not initialized\n");
        exit(0);
    }

    g_assert(gst_element_link_many(videosrc,imageFreeze,  NULL));
    g_assert(gst_element_link_filtered(imageFreeze, appsink, caps));
#else
    //gst_bin_add_many(GST_BIN(pipeline), videosrc, nvvidconv, imageFreeze, autofocus, barcodereader, queue, appsink, NULL);
    gst_bin_add_many(GST_BIN(pipeline), videosrc,imageFreeze, barcodereader, autofocus,autoexposure, appsink, NULL);

    if (!videosrc || !barcodereader || !appsink || !pipeline || !imageFreeze || !autofocus || !autoexposure)
    {
        printf("not initialized\n");
        exit(0);
    }


    g_assert(gst_element_link_many(videosrc,imageFreeze,barcodereader, autofocus,autoexposure, appsink, NULL));

    //g_assert(gst_element_link_filtered(videosrc, queue1, caps));


    g_object_set(G_OBJECT(autofocus), "listen", false, "debug_level", 2, NULL);

    g_object_set(G_OBJECT(barcodereader), "framing", false, NULL);
    g_object_set(G_OBJECT(autoexposure), "optimize", 2, NULL);
	/*
    g_object_set(G_OBJECT(appsink), "sync", 0, NULL);
	g_object_set(G_OBJECT(appsink), "drop", 1, NULL);
    g_object_set(G_OBJECT(appsink), "max-buffers", 1, NULL);*/
    g_object_set(G_OBJECT(filter), "caps", caps, NULL);
  gst_element_set_state(pipeline,GST_STATE_PLAYING);

#endif

    g_object_set(G_OBJECT(imageFreeze), "freeze", false, "listen", false, NULL);

    gst_caps_unref(caps);
}

GstSample *Pipeline::getSample()
{
	gst_app_sink_set_wait_on_eos(GST_APP_SINK(appsink),true);
    return gst_app_sink_try_pull_sample(GST_APP_SINK(appsink), GST_MSECOND*5);
}

void Pipeline::setState(GstState state)
{
    gst_element_set_state(pipeline, state);
}


GstElement *Pipeline::getAutofocus()
{
    return autofocus;

}
GstElement *Pipeline::getBarcodeReader()
{
    return barcodereader;
}

GstElement *Pipeline::getAutoexposure()
{
    return autoexposure;
}


void Pipeline::freezeStream(bool freeze)
{
    g_object_set(G_OBJECT(imageFreeze), "freeze", freeze, NULL);
}

void Pipeline::getVideoSize(int *width, int *height)
{
    GstSample *sample = gst_app_sink_pull_sample((GstAppSink *)(appsink));
    GstCaps *caps = gst_sample_get_caps(sample);
    GstStructure *s = gst_caps_get_structure(caps, 0);

    gboolean res;

    // we need to get the final caps on the buffer to get the size
    res = gst_structure_get_int(s, "width", width);
    res |= gst_structure_get_int(s, "height", height);

    if (!res)
    {
        g_print("Could not get snapshot dimension\n");
        exit(-1);
    }
gst_sample_unref(sample);
}