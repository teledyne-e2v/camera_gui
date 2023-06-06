#include "Pipeline.hpp"

#include <cstdio>
#include <fstream>
#include <gst/app/app.h>
Pipeline::Pipeline(int argc, char **argv) {
  gst_init(&argc, &argv);

  createElements();
  linkElementsCOLOR();
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

  GstElement *elements[] = {
      videosrc, imageFreeze, barcodereader, sharpness,  autofocus, autoexposure,
      multifocus,  whitebalance,  gray2bayer, bayer2rgb, appsink};
  const char *elements_name[] = {
      "v4l2src","imageFreeze", "barcodereader", "sharpness",  "autofocus", "autoexposure",
      "multifocus",  "whitebalance",  "gray2bayer", "bayer2rgb", "appsink"};
  for (unsigned int i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    if (elements[i]) {
      gst_bin_add(GST_BIN(pipeline), elements[i]);
    } else {
      printf("%s plugin not initialized\n", elements_name[i]);
    }
  }
}

void Pipeline::linkElementsGRAY() {

  GstElement *elements[] = {
      imageFreeze, barcodereader, sharpness,  autofocus, autoexposure,
      multifocus, appsink};

  GstElement *previousElement = videosrc;
  for (unsigned int i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
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

  /*
  g_object_set(G_OBJECT(appsink), "sync", 0, NULL);
  g_object_set(G_OBJECT(appsink), "drop", 1, NULL);
  g_object_set(G_OBJECT(appsink), "max-buffers", 1, NULL);*/
  // g_object_set(G_OBJECT(filter), "caps", caps, NULL);

  gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

void Pipeline::linkElementsCOLOR() {
  GstCaps *caps1 =
      gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "RGBA", NULL);
  GstCaps *caps2 = gst_caps_new_simple("video/x-bayer", "format", G_TYPE_STRING,
                                       "rggb", NULL);

  GstElement *elements[] = {
      imageFreeze, barcodereader, sharpness,  autofocus, autoexposure,
      multifocus,  whitebalance,  gray2bayer, bayer2rgb, appsink};


  GstElement *previousElement = videosrc;
  for (unsigned int i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
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
  gst_element_link_filtered(bayer2rgb, appsink, caps1);
  gst_element_link_filtered(gray2bayer, bayer2rgb, caps2);
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
  GstSample *sample = gst_app_sink_pull_sample((GstAppSink *)(appsink));
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
void Pipeline::switchToColor() {}

void Pipeline::switchToGRAY() {
  gst_element_set_state(pipeline, GST_STATE_NULL);
  linkElementsGRAY();
}