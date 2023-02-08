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
    queue = gst_element_factory_make("queue", "queue0");
    appsink = gst_element_factory_make("appsink", "videosink0");
    imageFreeze = gst_element_factory_make("freeze", "freeze0");

#ifdef DEBUG_MODE
    videosrc = gst_element_factory_make("videotestsrc", "videosrc0");
    nvvidconv = gst_element_factory_make("videoconvert", "nvvidconv0");
#else
    videosrc = gst_element_factory_make("nvarguscamerasrc", "videosrc0");
    nvvidconv = gst_element_factory_make("nvvidconv", "nvvidconv0");
    autofocus = gst_element_factory_make("autofocus", "autofocus0");
    barcodereader = gst_element_factory_make("barcodereader", "barcodereader0");
#endif
}

void Pipeline::linkElements()
{
    GstCaps *caps = gst_caps_new_simple("video/x-raw",
                                        "format", G_TYPE_STRING, "NV12",
                                        NULL);

#ifdef DEBUG_MODE
    gst_bin_add_many(GST_BIN(pipeline), videosrc, nvvidconv, imageFreeze, queue, appsink, NULL);

    if (!nvvidconv || !videosrc || !imageFreeze || !pipeline || !appsink || !nvvidconv || !queue)
    {
        printf("not initialized\n");
        exit(0);
    }

    g_assert(gst_element_link_many(videosrc, imageFreeze, nvvidconv, queue, NULL));
    g_assert(gst_element_link_filtered(queue, appsink, caps));
#else
    gst_bin_add_many(GST_BIN(pipeline), videosrc, nvvidconv, imageFreeze, autofocus, barcodereader, queue, appsink, NULL);

    if (!nvvidconv || !videosrc || !pipeline || !imageFreeze || !appsink || !nvvidconv || !autofocus || !queue || !barcodereader)
    {
        printf("not initialized\n");
        exit(0);
    }

    g_assert(gst_element_link_many(videosrc, nvvidconv, imageFreeze, autofocus, barcodereader, queue, NULL));
    g_assert(gst_element_link_filtered(queue, appsink, caps));

    g_object_set(G_OBJECT(videosrc), "aelock", true, "sensor-mode", 0, "saturation", 0, NULL);

    g_object_set(G_OBJECT(autofocus), "listen", false, "debug_level", 2, NULL);

    g_object_set(G_OBJECT(barcodereader), "framing", false, NULL);

    g_object_set(G_OBJECT(appsink), "max-buffers", 1, NULL);

#endif

    g_object_set(G_OBJECT(imageFreeze), "freeze", false, "listen", false, NULL);

    gst_caps_unref(caps);
}

GstSample *Pipeline::getSample()
{
    return gst_app_sink_try_pull_sample(GST_APP_SINK(appsink), 5 * GST_MSECOND);
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
}