#include "Pipeline.hpp"

#include <cstdio>
#include <fstream>
#include <gst/app/app.h>
Pipeline::Pipeline(int argc, char **argv) {
  gst_init(&argc, &argv);

  createElements();
  linkElementsGRAY();
}

Pipeline::~Pipeline() {
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);
}

void Pipeline::createElements() {
  pipeline = gst_pipeline_new("pipeline");
  queue1 = gst_element_factory_make("queue", "queue0");
  whitebalance = gst_element_factory_make("whitebalance", "whitebalance0");
  bayer2rgb = gst_element_factory_make("bayer2rgb", "bayer2rgb0");
  gray2bayer = gst_element_factory_make("gray2bayer", "gray2bayer0");
  queue2 = gst_element_factory_make("queue", "queue1");
  capsfilter =  gst_element_factory_make("capsfilter", "capsfilter0");

  appsink = gst_element_factory_make("appsink", "videosink0");

  imageFreeze = gst_element_factory_make("freeze", "freeze0");

  std::ifstream file("/dev/video0");
  if (!file.is_open()) {
    videosrc = gst_element_factory_make("videotestsrc", "videosrc0");
  } else {
    videosrc = gst_element_factory_make("v4l2src", "videosrc0");
    autoexposure = gst_element_factory_make("autoexposure", "autoexposure0");
    autofocus = gst_element_factory_make("autofocus", "autofocus0");
    multifocus = gst_element_factory_make("multifocus", "multifocus0");
    sharpness = gst_element_factory_make("sharpness", "sharpness0");
    file.close();
  }
  barcodereader = gst_element_factory_make("barcodereader", "barcodereader0");

  GstElement *elements[] = {videosrc,   imageFreeze,  barcodereader,
                            sharpness,  autofocus,    autoexposure,
                            multifocus, /*whitebalance, gray2bayer, bayer2rgb,*/ capsfilter,  appsink};

  for (unsigned int i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    if (elements[i]) {
      gst_bin_add(GST_BIN(pipeline), elements[i]);
    } else {
      printf("%s plugin not initialized\n", gst_element_get_name(elements[i]));
    }
  }
}

void Pipeline::linkElementsGRAY() {

  GstElement *elements[] = {imageFreeze,  barcodereader, sharpness, autofocus,
                            autoexposure, multifocus, capsfilter};

  GstCaps *caps1 =
      gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "GRAY8", NULL);


  GstElement *previousElement = videosrc;
  for (unsigned int i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    if (elements[i]) {
      printf("%s -> %s \n",gst_element_get_name(previousElement),gst_element_get_name(elements[i]));

      g_assert(gst_element_link(previousElement, elements[i]));

      previousElement = elements[i];
    }
  }
  g_assert(gst_element_link(capsfilter, appsink));

  if (autofocus) {
    g_object_set(G_OBJECT(autofocus), "listen", false, "debug_level", 2, NULL);
  }
  if (sharpness) {
    g_object_set(G_OBJECT(sharpness), "work", false, NULL);
  }
  if (barcodereader) {
    g_object_set(G_OBJECT(barcodereader), "framing", false, NULL);
  }

  if (autoexposure) {
    g_object_set(G_OBJECT(autoexposure), "optimize", 2, NULL);
  }

  if (multifocus) {
    g_object_set(G_OBJECT(multifocus), "work", false, NULL);
  }
  if (imageFreeze) {
    g_object_set(G_OBJECT(imageFreeze), "freeze", false, "listen", false, NULL);
  }
   g_object_set(G_OBJECT(capsfilter), "caps", caps1, NULL);
  /*
  g_object_set(G_OBJECT(appsink), "sync", 0, NULL);
  g_object_set(G_OBJECT(appsink), "drop", 1, NULL);
  g_object_set(G_OBJECT(appsink), "max-buffers", 1, NULL);*/
  // g_object_set(G_OBJECT(filter), "caps", caps, NULL);
  gst_caps_unref(caps1);

  gst_element_set_state(pipeline, GST_STATE_PLAYING);

}

void Pipeline::linkElementsCOLOR() {
  GstCaps *caps1 =
      gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "RGBA", NULL);
  GstCaps *caps2 = gst_caps_new_simple("video/x-bayer", "format", G_TYPE_STRING,
                                       "rggb", NULL);

  GstElement *elements[] = {
      imageFreeze, barcodereader, sharpness,  autofocus, autoexposure,
      multifocus,  whitebalance,  gray2bayer, bayer2rgb,capsfilter,appsink};

  GstElement *previousElement = videosrc;
  for (unsigned int i = 0; i < sizeof(elements) / sizeof(elements[0]) - 2;
       i++) {
    if (elements[i]) {
      g_assert(gst_element_link(previousElement, elements[i]));
      previousElement = elements[i];
    }
  }
  if (autofocus) {
    g_object_set(G_OBJECT(autofocus), "listen", false, "debug_level", 2, NULL);
  }
  if (sharpness) {
    g_object_set(G_OBJECT(sharpness), "work", false, NULL);
  }
  if (barcodereader) {
    g_object_set(G_OBJECT(barcodereader), "framing", false, NULL);
  }

  if (autoexposure) {
    g_object_set(G_OBJECT(autoexposure), "optimize", 2, NULL);
  }

  if (multifocus) {
    g_object_set(G_OBJECT(multifocus), "work", false, NULL);
  }
  if (imageFreeze) {
    g_object_set(G_OBJECT(imageFreeze), "freeze", false, "listen", false, NULL);
  }
  g_object_set(G_OBJECT(capsfilter), "caps", caps1, NULL);

  gst_element_link(gray2bayer, bayer2rgb);
  gst_element_link(bayer2rgb, appsink);
  gst_caps_unref(caps1);

  //gst_caps_unref(caps1);
  //gst_caps_unref(caps2);
  gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

GstSample *Pipeline::getSample() {

    gst_app_sink_set_wait_on_eos(GST_APP_SINK(appsink), true);
  return gst_app_sink_try_pull_sample(GST_APP_SINK(appsink), GST_MSECOND * 5);


}

void Pipeline::setState(GstState state) {
  gst_element_set_state(pipeline, state);
}

GstElement *Pipeline::getAutofocus() { return autofocus; }

GstElement *Pipeline::getImageFreeze() { return imageFreeze; }

GstElement *Pipeline::getBarcodeReader() { return barcodereader; }

GstElement *Pipeline::getSharpness() { return sharpness; }

GstElement *Pipeline::getAutoexposure() { return autoexposure; }

GstElement *Pipeline::getMultifocus() { return multifocus; }

void Pipeline::freezeStream(bool freeze) {
  g_object_set(G_OBJECT(imageFreeze), "freeze", freeze, NULL);
}

void Pipeline::getVideoSize(int *width, int *height) {
  GstSample *sample;

  sample = gst_app_sink_pull_sample((GstAppSink *)(appsink));


  GstCaps *caps = gst_sample_get_caps(sample);
  GstStructure *s = gst_caps_get_structure(caps, 0);

  gboolean res;

  // we need to get the final caps on the buffer to get the size
  res = gst_structure_get_int(s, "width", width);
  res |= gst_structure_get_int(s, "height", height);

  if (!res) {
    g_print("Could not get snapshot dimension\n");
    exit(-1);
  }
  gst_sample_unref(sample);
}



typedef struct _CustomData{
  GstPad *blockpad;
  GstElement *pipeline;

  GstElement *capsfilter;
  GstElement *appsink;
  GstElement *whitebalance;
  GstElement *bayer2rgb;
  GstElement *gray2bayer;
}CustomData;

static GstPadProbeReturn pad_probe_cb_color(GstPad *pad, GstPadProbeInfo * info, gpointer user_data)
{
  CustomData *data =(CustomData*) user_data;
  gst_bin_add_many(GST_BIN(data->pipeline),data->whitebalance,data->gray2bayer,data->bayer2rgb,NULL);
  gst_element_link_many(data->capsfilter,data->whitebalance,data->gray2bayer,data->bayer2rgb,data->appsink,NULL);
  gst_element_set_state(data->appsink,GST_STATE_PLAYING);
  gst_pad_remove_probe(pad,GST_PAD_PROBE_INFO_ID (info));
  return GST_PAD_PROBE_REMOVE;
}


void Pipeline::switchToColor() {
        GstCaps *caps1 =
      gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "RGBA", NULL);
    g_object_set(G_OBJECT(capsfilter), "caps", caps1, NULL);
  gst_caps_unref(caps1);
  CustomData data;
  data.appsink=appsink;
  data.pipeline=pipeline;
  data.capsfilter=capsfilter;
  data.whitebalance=whitebalance;
  data.bayer2rgb=bayer2rgb;
  data.gray2bayer=gray2bayer;
  data.blockpad=gst_element_get_static_pad(data.capsfilter,"src");
  gst_pad_add_probe(data.blockpad,(GstPadProbeType)(GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM | GST_PAD_PROBE_TYPE_BUFFER),pad_probe_cb_color,&data, NULL);
  
  auto sinkpad = gst_element_get_static_pad (data.capsfilter, "sink");
  gst_pad_send_event (sinkpad, gst_event_new_eos ());
}


/*
void Pipeline::switchToColor() {
  color=true;
      GstCaps *caps1 =
      gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "RGBA", NULL);
    g_object_set(G_OBJECT(capsfilter), "caps", caps1, NULL);
  gst_caps_unref(caps1);
  gst_element_set_state(pipeline, GST_STATE_PAUSED);

  GstElement *elements[] = {imageFreeze,  barcodereader, sharpness, autofocus,
                            autoexposure, multifocus, capsfilter, appsink};

  GstElement *previousElement = videosrc;
  for (unsigned int i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    if (elements[i]) {
      gst_element_unlink(previousElement, elements[i]);
      previousElement = elements[i];
    }
  }
  //gst_bin_remove(GST_BIN(pipeline),appsink);
  //gst_bin_add(GST_BIN(pipeline),appsink);
 
  linkElementsCOLOR();
}*/
void Pipeline::switchToGRAY() {
  gst_event_new_flush_start();

color=false;
    GstCaps *caps1 =
      gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "GRAY8", NULL);
    g_object_set(G_OBJECT(capsfilter), "caps", caps1, NULL);
  gst_caps_unref(caps1);
  gst_element_set_state(pipeline, GST_STATE_PAUSED);
  GstElement *elements[] = {
      imageFreeze, barcodereader, sharpness,  autofocus, autoexposure,
      multifocus,  whitebalance,  gray2bayer, bayer2rgb,capsfilter, appsink};

  GstElement *previousElement = videosrc;
  for (unsigned int i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    if (elements[i]) {
      printf("unlink : %s -> %s \n",gst_element_get_name(previousElement),gst_element_get_name(elements[i]));

      gst_element_unlink(previousElement, elements[i]);
      previousElement = elements[i];
    }
  }

  //gst_bin_remove(GST_BIN(pipeline),appsink);
  //gst_bin_add(GST_BIN(pipeline),appsink);
  linkElementsGRAY();
  gst_event_new_flush_stop(true);

}