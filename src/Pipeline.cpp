#include "Pipeline.hpp"

#include <cstdio>
#include <fstream>
#include <gst/app/app.h>
#include <string>

static const std::string ELEMENTS_COLOR[] = {"whitebalance", "gray2bayer",
                                             "bayer2rgb"};

static const std::string ELEMENTS_GRAY[] = {"v4l2src", "freeze",
                                            "barcodereader", "autoexposure"};
static const std::string ELEMENTS_QUEUE[] = {"queue", "fpscounter"};
static const std::string ELEMENTS_MULTIFOCUS[] = {"multifocus", "autofocus",
                                                  "sharpness"};
static const std::string ELEMENT_SINK = "appsink";

Pipeline::Pipeline(int argc, char **argv, bool color, bool multifocus)
{
  gst_init(&argc, &argv);

  createElements(color, multifocus);
  linkElementsGRAY();
}

Pipeline::~Pipeline()
{
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);
}

void Pipeline::mapping_Gst_Element(std::string element_name)
{
  GstElements[element_name] = gst_element_factory_make(
      element_name.c_str(), (element_name + std::string("0")).c_str());
}

void Pipeline::createElements(bool color, bool multifocus)
{

  pipeline = gst_pipeline_new("pipeline");

  for (unsigned int i = 0; i < sizeof(ELEMENTS_GRAY) / sizeof(ELEMENTS_GRAY[0]);
       i++)
  {
    mapping_Gst_Element(ELEMENTS_GRAY[i]);
  }
  if (multifocus)
  {
    for (unsigned int i = 0;
         i < sizeof(ELEMENTS_MULTIFOCUS) / sizeof(ELEMENTS_MULTIFOCUS[0]);
         i++)
    {
      mapping_Gst_Element(ELEMENTS_MULTIFOCUS[i]);
    }
  }

  for (unsigned int i = 0;
       i < sizeof(ELEMENTS_QUEUE) / sizeof(ELEMENTS_QUEUE[0]); i++)
  {
    mapping_Gst_Element(ELEMENTS_QUEUE[i]);
  }
  mapping_Gst_Element(ELEMENT_SINK);

  for (std::map<std::string, GstElement *>::iterator it = GstElements.begin();
       it != GstElements.end(); ++it)
  {
    if (it->second)
    {
      gst_bin_add(GST_BIN(pipeline), it->second);
    }
    else
    {
      printf("%s plugin not initialized\n", it->first.c_str());
    }
  }
  color_support = color;
  if (color)
  {
    for (unsigned int i = 0;
         i < sizeof(ELEMENTS_COLOR) / sizeof(ELEMENTS_COLOR[0]); i++)
    {
      mapping_Gst_Element(ELEMENTS_COLOR[i]);
      if (!GstElements[ELEMENTS_COLOR[i]])
      {
        printf("%s plugin not initialized, install it for color support\n",
               ELEMENTS_COLOR[i].c_str());
        color_support = false;
      }
    }
  }

  blockpad = gst_element_get_static_pad(GstElements["queue"], "src");
  GstElements["pipeline"] = pipeline;
}

void Pipeline::linkElementsGRAY()
{

  GstElement *previousElement = GstElements[ELEMENTS_GRAY[0]];

  for (unsigned int i = 1; i < sizeof(ELEMENTS_GRAY) / sizeof(ELEMENTS_GRAY[0]);
       i++)
  {
    if (GstElements[ELEMENTS_GRAY[i]])
    {

      printf("%s -> %s \n", gst_element_get_name(previousElement),
             gst_element_get_name(GstElements[ELEMENTS_GRAY[i]]));
      g_assert(
          gst_element_link(previousElement, GstElements[ELEMENTS_GRAY[i]]));
      previousElement = GstElements[ELEMENTS_GRAY[i]];
    }
  }

  for (unsigned int i = 0;
       i < sizeof(ELEMENTS_MULTIFOCUS) / sizeof(ELEMENTS_MULTIFOCUS[0]); i++)
  {
    if (GstElements[ELEMENTS_MULTIFOCUS[i]])
    {

      printf("%s -> %s \n", gst_element_get_name(previousElement),
             gst_element_get_name(GstElements[ELEMENTS_MULTIFOCUS[i]]));
      g_assert(gst_element_link(previousElement,
                                GstElements[ELEMENTS_MULTIFOCUS[i]]));
      previousElement = GstElements[ELEMENTS_MULTIFOCUS[i]];
    }
  }

  for (unsigned int i = 0;
       i < sizeof(ELEMENTS_QUEUE) / sizeof(ELEMENTS_QUEUE[0]); i++)
  {
    if (GstElements[ELEMENTS_QUEUE[i]])
    {

      printf("%s -> %s \n", gst_element_get_name(previousElement),
             gst_element_get_name(GstElements[ELEMENTS_QUEUE[i]]));
      g_assert(
          gst_element_link(previousElement, GstElements[ELEMENTS_QUEUE[i]]));
      previousElement = GstElements[ELEMENTS_QUEUE[i]];
    }
  }

  g_assert(gst_element_link(previousElement, GstElements[ELEMENT_SINK]));

  if (GstElements["autofocus"])
  {
    g_object_set(G_OBJECT(GstElements["autofocus"]), "listen", false,
                 "debug_level", 2, NULL);
  }
  if (GstElements["sharpness"])
  {
    g_object_set(G_OBJECT(GstElements["sharpness"]), "work", false, NULL);
  }
  if (GstElements["barcodereader"])
  {
    g_object_set(G_OBJECT(GstElements["barcodereader"]), "framing", false,
                 NULL);
  }

  if (GstElements["autoexposure"])
  {
    g_object_set(G_OBJECT(GstElements["autoexposure"]), "optimize", 2, NULL);
  }

  if (GstElements["multifocus"])
  {
    g_object_set(G_OBJECT(GstElements["multifocus"]), "work", false, NULL);
  }
  if (GstElements["imageFreeze"])
  {
    g_object_set(G_OBJECT(GstElements["imageFreeze"]), "freeze", false,
                 "listen", false, NULL);
  }
  if (GstElements["fpscounter"])
  {
    g_object_set(G_OBJECT(GstElements["fpscounter"]), "silent", true, NULL);
  }

  g_object_set(G_OBJECT(GstElements["appsink"]), "sync", 0, NULL);
  g_object_set(G_OBJECT(GstElements["appsink"]), "drop", 1, NULL);
  g_object_set(G_OBJECT(GstElements["appsink"]), "max-buffers", 1, NULL);

  gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

GstSample *Pipeline::getSample()
{

  gst_app_sink_set_wait_on_eos(GST_APP_SINK(GstElements[ELEMENT_SINK]), true);
  return gst_app_sink_try_pull_sample(GST_APP_SINK(GstElements[ELEMENT_SINK]),
                                      GST_MSECOND * 5);
}

void Pipeline::setState(GstState state)
{
  gst_element_set_state(pipeline, state);
}

GstElement *Pipeline::getAutofocus() { return GstElements["autofocus"]; }

GstElement *Pipeline::getFpscounter() { return GstElements["fpscounter"]; }

GstElement *Pipeline::getImageFreeze() { return GstElements["freeze"]; }

GstElement *Pipeline::getWhiteBalance() { return GstElements["whitebalance"]; }

GstElement *Pipeline::getBarcodeReader()
{
  return GstElements["barcodereader"];
}

GstElement *Pipeline::getSharpness() { return GstElements["sharpness"]; }

GstElement *Pipeline::getAutoexposure() { return GstElements["autoexposure"]; }

GstElement *Pipeline::getMultifocus() { return GstElements["multifocus"]; }

void Pipeline::freezeStream(bool freeze)
{
  g_object_set(G_OBJECT(GstElements["freeze"]), "freeze", freeze, NULL);
}

void Pipeline::getVideoSize(int *width, int *height)
{
  GstSample *sample;

  sample = gst_app_sink_pull_sample((GstAppSink *)(GstElements[ELEMENT_SINK]));

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

static GstPadProbeReturn
event_probe_cb_color(GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
  std::map<std::string, GstElement *> GstElements =
      (*((std::map<std::string, GstElement *> *)((struct Data *)user_data)->GstElements));
  GstElement *pipeline = GstElements["pipeline"];
  GstElement *fpscounter = GstElements["fpscounter"];
  GstElement *whitebalance = GstElements["whitebalance"];
  GstElement *gray2bayer = GstElements["gray2bayer"];
  GstElement *bayer2rgb = GstElements["bayer2rgb"];
  GstElement *queue = GstElements["queue"];



  if (GST_EVENT_TYPE(GST_PAD_PROBE_INFO_DATA(info)) != GST_EVENT_EOS)
    return GST_PAD_PROBE_OK;

  gst_pad_remove_probe(pad, GST_PAD_PROBE_INFO_ID(info));

  gst_element_set_state(fpscounter, GST_STATE_NULL);

  GstCaps *caps = gst_caps_new_simple("video/x-bayer", "format", G_TYPE_STRING,
                                      ((struct Data *) user_data)->pixels, NULL);
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
  gst_element_link_many(queue, fpscounter, whitebalance, gray2bayer, NULL);

  gst_element_link_filtered(gray2bayer, bayer2rgb, caps);
  gst_element_link(bayer2rgb, GstElements[ELEMENT_SINK]);

  gst_element_set_state(fpscounter, GST_STATE_PLAYING);
  gst_element_set_state(whitebalance, GST_STATE_PLAYING);
  gst_element_set_state(gray2bayer, GST_STATE_PLAYING);
  gst_element_set_state(bayer2rgb, GST_STATE_PLAYING);

  GST_DEBUG_OBJECT(pipeline, "done");

  gst_caps_unref(caps);

  return GST_PAD_PROBE_DROP;
}

static GstPadProbeReturn pad_probe_cb_color(GstPad *pad, GstPadProbeInfo *info,
                                            gpointer user_data)
{

  GstElement *fpscounter =
      ((*((std::map<std::string, GstElement *> *)((struct Data *)user_data)->GstElements))["fpscounter"]);
  GST_DEBUG_OBJECT(pad, "pad is blocked now");
    printf("CC\n");

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

static GstPadProbeReturn event_probe_cb_gray(GstPad *pad, GstPadProbeInfo *info,
                                             gpointer user_data)
{

  std::map<std::string, GstElement *> GstElements =
      (*((std::map<std::string, GstElement *> *)user_data));
  GstElement *pipeline = GstElements["pipeline"];
  if (GST_EVENT_TYPE(GST_PAD_PROBE_INFO_DATA(info)) != GST_EVENT_EOS)
    return GST_PAD_PROBE_OK;

  gst_pad_remove_probe(pad, GST_PAD_PROBE_INFO_ID(info));

  gst_element_set_state(GstElements["whitebalance"], GST_STATE_NULL);

  /* remove unlinks automatically */
  GST_DEBUG_OBJECT(pipeline, "removing %" GST_PTR_FORMAT,
                   GstElements["whitebalance"]);
  gst_bin_remove(GST_BIN(pipeline), GstElements["whitebalance"]);

  GST_DEBUG_OBJECT(pipeline, "removing %" GST_PTR_FORMAT,
                   GstElements["gray2bayer"]);
  gst_bin_remove(GST_BIN(pipeline), GstElements["gray2bayer"]);

  GST_DEBUG_OBJECT(pipeline, "removing %" GST_PTR_FORMAT,
                   GstElements["bayer2rgb"]);
  gst_bin_remove(GST_BIN(pipeline), GstElements["bayer2rgb"]);

  GST_DEBUG_OBJECT(pipeline, "removing %" GST_PTR_FORMAT,
                   GstElements["fpscounter"]);
  gst_bin_remove(GST_BIN(pipeline), GstElements["fpscounter"]);

  /* add, link and start the new effect */
  GST_DEBUG_OBJECT(pipeline, "adding   %" GST_PTR_FORMAT,
                   GstElements["fpscounter"]);
  gst_bin_add(GST_BIN(pipeline), GstElements["fpscounter"]);

  GST_DEBUG_OBJECT(pipeline, "linking..");
  gst_element_link_many(GstElements["queue"], GstElements["fpscounter"],
                        GstElements[ELEMENT_SINK], NULL);

  gst_element_set_state(GstElements["fpscounter"], GST_STATE_PLAYING);

  GST_DEBUG_OBJECT(pipeline, "done");

  return GST_PAD_PROBE_DROP;
}

static GstPadProbeReturn pad_probe_cb_gray(GstPad *pad, GstPadProbeInfo *info,
                                           gpointer user_data)
{

  GstPad *srcpad, *sinkpad;

  /* remove the probe first */
  gst_pad_remove_probe(pad, GST_PAD_PROBE_INFO_ID(info));

  /* install new probe for EOS */
  srcpad = gst_element_get_static_pad(
      (*((std::map<std::string, GstElement *> *)user_data))["whitebalance"],
      "src");
  gst_pad_add_probe(
      srcpad,
      (GstPadProbeType)(((int)GST_PAD_PROBE_TYPE_BLOCK) |
                        ((int)GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM)),
      event_probe_cb_gray, user_data, NULL);
  gst_object_unref(srcpad);

  /* push EOS into the element, the probe will be fired when the
   * EOS leaves the effect and it has thus drained all of its data */
  sinkpad = gst_element_get_static_pad(
      (*((std::map<std::string, GstElement *> *)user_data))["whitebalance"],
      "sink");
  gst_pad_send_event(sinkpad, gst_event_new_eos());
  gst_object_unref(sinkpad);

  return GST_PAD_PROBE_OK;
}

void Pipeline::switchToColor(const char* pixels)
{

  data.GstElements = &GstElements;
  strncpy(data.pixels, pixels, 5);
  gpointer user_data = &data;

  printf("switch to color\n");
  gst_pad_add_probe(blockpad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM,
                    pad_probe_cb_color, user_data, NULL);
}

void Pipeline::switchToGRAY()
{
  gpointer user_data = &GstElements;
  printf("switch to gray\n");
  gst_pad_add_probe(blockpad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM,
                    pad_probe_cb_gray, user_data, NULL);
}