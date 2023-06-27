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
  fpscounter = gst_element_factory_make("fpscounter", "fpscounter0");
  appsink = gst_element_factory_make("appsink", "videosink0");

  blockpad = gst_element_get_static_pad(queue1, "src");

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

  GstElement *elements[] = {videosrc,
                            imageFreeze,
                            barcodereader,
                            sharpness,
                            autofocus,
                            autoexposure,
                            multifocus,
                            queue1,
                            /*whitebalance, gray2bayer, bayer2rgb,*/ fpscounter,
                            appsink};

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
                            autoexposure, multifocus,    queue1,    fpscounter};

  GstCaps *caps1 = gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING,
                                       "GRAY8", NULL);

  GstElement *previousElement = videosrc;
  for (unsigned int i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    if (elements[i]) {
      printf("%s -> %s \n", gst_element_get_name(previousElement),
             gst_element_get_name(elements[i]));

      g_assert(gst_element_link(previousElement, elements[i]));

      previousElement = elements[i];
    }
  }
  g_assert(gst_element_link(fpscounter, appsink));

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

GstSample *Pipeline::getSample() {

  gst_app_sink_set_wait_on_eos(GST_APP_SINK(appsink), true);
  return gst_app_sink_try_pull_sample(GST_APP_SINK(appsink), GST_MSECOND * 5);
}

void Pipeline::setState(GstState state) {
  gst_element_set_state(pipeline, state);
}

GstElement *Pipeline::getAutofocus() { return autofocus; }

GstElement *Pipeline::getFpscounter() { return fpscounter; }

GstElement *Pipeline::getImageFreeze() { return imageFreeze; }

GstElement *Pipeline::getWhiteBalance() { return whitebalance; }

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

typedef struct _CustomData {
  GstPad *blockpad;
  GstElement *pipeline;

  GstElement *capsfilter;
  GstElement *appsink;
  GstElement *whitebalance;
  GstElement *bayer2rgb;
  GstElement *gray2bayer;
} CustomData;


static GstPadProbeReturn
event_probe_cb_color(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {

  GstCaps *caps = gst_caps_new_simple("video/x-bayer","format", G_TYPE_STRING,"rggb");

  if (GST_EVENT_TYPE(GST_PAD_PROBE_INFO_DATA(info)) != GST_EVENT_EOS)
    return GST_PAD_PROBE_OK;

  gst_pad_remove_probe(pad, GST_PAD_PROBE_INFO_ID(info));

  g_print("Switching from '%s' to '%s'..\n", GST_OBJECT_NAME(fpscounter),
          GST_OBJECT_NAME(whitebalance));

  gst_element_set_state(fpscounter, GST_STATE_NULL);

  /* remove unlinks automatically */
  GST_DEBUG_OBJECT(pipeline, "removing %" GST_PTR_FORMAT, fpscounter);
  gst_bin_remove(GST_BIN(pipeline), fpscounter);

  /* add, link and start the new effect */

    GST_DEBUG_OBJECT(pipeline, "adding   %" GST_PTR_FORMAT, fpscounter);
  gst_bin_add(GST_BIN(pipeline), fpscounter);

  GST_DEBUG_OBJECT(pipeline, "adding   %" GST_PTR_FORMAT, whitebalance);
  gst_bin_add(GST_BIN(pipeline), whitebalance);

  GST_DEBUG_OBJECT(pipeline, "adding   %" GST_PTR_FORMAT, gray2bayer);
  gst_bin_add(GST_BIN(pipeline), gray2bayer);

  GST_DEBUG_OBJECT(pipeline, "adding   %" GST_PTR_FORMAT, bayer2rgb);
  gst_bin_add(GST_BIN(pipeline), bayer2rgb);

  GST_DEBUG_OBJECT(pipeline, "linking..");
  gst_element_link_many(queue1,fpscounter, whitebalance, gray2bayer, NULL);

  gst_element_link_filtered(gray2bayer,bayer2rgb,caps);
  gst_element_link(bayer2rgb, appsink);

  gst_element_set_state(fpscounter, GST_STATE_PLAYING);
  gst_element_set_state(whitebalance, GST_STATE_PLAYING);
  gst_element_set_state(gray2bayer, GST_STATE_PLAYING);
  gst_element_set_state(bayer2rgb, GST_STATE_PLAYING);

  GST_DEBUG_OBJECT(pipeline, "done");
  gst_caps_unref(caps);
  return GST_PAD_PROBE_DROP;
}

static GstPadProbeReturn pad_probe_cb_color(GstPad *pad, GstPadProbeInfo *info,
                                            gpointer user_data) {

  GstPad *srcpad, *sinkpad;

  GST_DEBUG_OBJECT(pad, "pad is blocked now");

  /* remove the probe first */
  gst_pad_remove_probe(pad, GST_PAD_PROBE_INFO_ID(info));

  /* install new probe for EOS */
  srcpad = gst_element_get_static_pad(fpscounter, "src");
  gst_pad_add_probe(
      srcpad,
      (GstPadProbeType)(((int)GST_PAD_PROBE_TYPE_BLOCK) |
                        ((int)GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM)),
      event_probe_cb_color, user_data, NULL);
  gst_object_unref(srcpad);

  /* push EOS into the element, the probe will be fired when the
   * EOS leaves the effect and it has thus drained all of its data */
  sinkpad = gst_element_get_static_pad(fpscounter, "sink");
  gst_pad_send_event(sinkpad, gst_event_new_eos());
  gst_object_unref(sinkpad);

  return GST_PAD_PROBE_OK;
}


static GstPadProbeReturn
event_probe_cb_gray(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {

  if (GST_EVENT_TYPE(GST_PAD_PROBE_INFO_DATA(info)) != GST_EVENT_EOS)
    return GST_PAD_PROBE_OK;

  gst_pad_remove_probe(pad, GST_PAD_PROBE_INFO_ID(info));

  g_print("Switching from '%s' to '%s'..\n", GST_OBJECT_NAME(whitebalance),
          GST_OBJECT_NAME(fpscounter));

  gst_element_set_state(whitebalance, GST_STATE_NULL);

  /* remove unlinks automatically */
  GST_DEBUG_OBJECT(pipeline, "removing %" GST_PTR_FORMAT, whitebalance);
  gst_bin_remove(GST_BIN(pipeline), whitebalance);

    GST_DEBUG_OBJECT(pipeline, "removing %" GST_PTR_FORMAT, gray2bayer);
  gst_bin_remove(GST_BIN(pipeline), gray2bayer);

    GST_DEBUG_OBJECT(pipeline, "removing %" GST_PTR_FORMAT, bayer2rgb);
  gst_bin_remove(GST_BIN(pipeline), bayer2rgb);

  GST_DEBUG_OBJECT(pipeline, "removing %" GST_PTR_FORMAT, fpscounter);
  gst_bin_remove(GST_BIN(pipeline), fpscounter);
  
  /* add, link and start the new effect */
  GST_DEBUG_OBJECT(pipeline, "adding   %" GST_PTR_FORMAT, fpscounter);
  gst_bin_add(GST_BIN(pipeline), fpscounter);


  GST_DEBUG_OBJECT(pipeline, "linking..");
  gst_element_link_many(queue1, fpscounter, appsink,
                        NULL);

  gst_element_set_state(fpscounter, GST_STATE_PLAYING);

  GST_DEBUG_OBJECT(pipeline, "done");

  return GST_PAD_PROBE_DROP;
}

static GstPadProbeReturn pad_probe_cb_gray(GstPad *pad, GstPadProbeInfo *info,
                                            gpointer user_data) {

  GstPad *srcpad, *sinkpad;

  GST_DEBUG_OBJECT(pad, "pad is blocked now");

  /* remove the probe first */
  gst_pad_remove_probe(pad, GST_PAD_PROBE_INFO_ID(info));

  /* install new probe for EOS */
  srcpad = gst_element_get_static_pad(whitebalance, "src");
  gst_pad_add_probe(
      srcpad,
      (GstPadProbeType)(((int)GST_PAD_PROBE_TYPE_BLOCK) |
                        ((int)GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM)),
      event_probe_cb_gray, user_data, NULL);
  gst_object_unref(srcpad);

  /* push EOS into the element, the probe will be fired when the
   * EOS leaves the effect and it has thus drained all of its data */
  sinkpad = gst_element_get_static_pad(whitebalance, "sink");
  gst_pad_send_event(sinkpad, gst_event_new_eos());
  gst_object_unref(sinkpad);

  return GST_PAD_PROBE_OK;
}

void Pipeline::switchToColor() {
  gpointer user_data;
  printf("switch to color\n");
  gst_pad_add_probe(blockpad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM,
                    pad_probe_cb_color, user_data, NULL);
}

void Pipeline::switchToGRAY() {
  gpointer user_data;
  printf("switch to color\n");
  gst_pad_add_probe(blockpad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM,
                    pad_probe_cb_gray, user_data, NULL);
}