#include "Pipeline.hpp"

#include <gst/app/app.h>
#include <cstdio>
#include <fstream>
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

    appsink = gst_element_factory_make("appsink", "videosink0");
    imageFreeze = gst_element_factory_make("freeze", "freeze0");

    std::ifstream file("/dev/video0");
    if(!file.is_open()){
        videosrc = gst_element_factory_make("videotestsrc", "videosrc0");}
else{
    videosrc = gst_element_factory_make("v4l2src", "videosrc0");
    autoexposure = gst_element_factory_make("autoexposure", "autoexposure0");
    autofocus = gst_element_factory_make("autofocus", "autofocus0");
    multifocus = gst_element_factory_make("multifocus", "multifocus0");
    sharpness = gst_element_factory_make("sharpness", "sharpness0");
    file.close();
    }

    barcodereader = gst_element_factory_make("barcodereader", "barcodereader0");
}

void Pipeline::linkElements()
{




    GstElement *element[]={imageFreeze, barcodereader, autofocus, autoexposure, multifocus,sharpness, appsink};
gst_bin_add(GST_BIN(pipeline), videosrc);
for(int i=0;i<7;i++)
	{
		if(element[i])
		{	
			gst_bin_add(GST_BIN(pipeline), element[i]);
		}
	}


    if(!videosrc)
    {
        printf("v4l2src plugin not initialized\n");
        exit(0);
    }
    if(!barcodereader)
    {
        printf("barcodereader plugin not initialized\n");
    }
    if(!appsink)
    {
        printf("appsink plugin not initialized\n");
        exit(0);
    }
    if(!imageFreeze)
    {
        printf("imageFreeze plugin not initialized\n");
    }
    if(!autofocus)
    {
        printf("autofocus plugin not initialized\n");
    }
    if(!multifocus)
    {
        printf("multifocus plugin not initialized\n");
    }
    if(!autoexposure)
    {
        printf("autoexposure plugin not initialized\n");
    }
    if(!sharpness)
    {
        printf("autoexposure plugin not initialized\n");
    }
    if(!pipeline)
    {
        printf("pipeline not initialized\n");
        exit(0);
    }


    GstElement *previousElement = videosrc;
	for(int i=0;i<6;i++)
	{
		if(element[i])
		{
			g_assert(gst_element_link(previousElement,element[i]));
			previousElement=element[i];
		}
	}
    if(autofocus)
{
    g_object_set(G_OBJECT(autofocus), "listen", false, "debug_level", 2, NULL);
}
 if(sharpness)
{
    g_object_set(G_OBJECT(autofocus), "work", false, NULL);
}
	if(barcodereader)
{
    g_object_set(G_OBJECT(barcodereader), "framing", false, NULL);
}

	if(autoexposure)
{
    g_object_set(G_OBJECT(autoexposure), "optimize", 2, NULL);
}

	if(multifocus)
{
    g_object_set(G_OBJECT(multifocus), "work", false, NULL);
}
	if(imageFreeze)
{
    g_object_set(G_OBJECT(imageFreeze), "freeze", false, "listen", false, NULL);
}



    /*
    g_object_set(G_OBJECT(appsink), "sync", 0, NULL);
    g_object_set(G_OBJECT(appsink), "drop", 1, NULL);
    g_object_set(G_OBJECT(appsink), "max-buffers", 1, NULL);*/
    //g_object_set(G_OBJECT(filter), "caps", caps, NULL);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);	


    
}

GstSample *Pipeline::getSample()
{
    gst_app_sink_set_wait_on_eos(GST_APP_SINK(appsink), true);
    return gst_app_sink_try_pull_sample(GST_APP_SINK(appsink), GST_MSECOND * 5);
}

void Pipeline::setState(GstState state)
{
    gst_element_set_state(pipeline, state);
}

GstElement *Pipeline::getAutofocus()
{
    return autofocus;
}

GstElement *Pipeline::getImageFreeze()
{
    return imageFreeze;
}

GstElement *Pipeline::getBarcodeReader()
{
    return barcodereader;
}

GstElement *Pipeline::getSharpness()
{
    return sharpness;
}

GstElement *Pipeline::getAutoexposure()
{
    return autoexposure;
}

GstElement *Pipeline::getMultifocus()
{
    return multifocus;
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
