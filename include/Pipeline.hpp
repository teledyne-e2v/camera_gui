#pragma once

#include <gst/gst.h>

/**
 * @brief Class creating and working on the pipeline
 *
 */
class Pipeline {
public:
  /**
   * @brief Construct a new Pipeline object
   *
   * @param argc
   * @param argv
   */
  Pipeline(int argc, char **argv);
  ~Pipeline();
  /**
   * @brief Get the Sample object
   *
   * @return GstSample*
   */
  GstSample *getSample();
  /**
   * @brief Set the State object
   *
   * @param state
   */
  void setState(GstState state);
  /**
   * @brief Get the Autofocus object
   *
   * @return GstElement*
   */
  GstElement *getAutofocus();
  /**
   * @brief Get the Barcode Reader object
   *
   * @return GstElement*
   */
  GstElement *getBarcodeReader();
  GstElement *getSharpness();
  GstElement *getFpscounter();
  GstElement *getWhiteBalance();

  /**
   * @brief Get the Autoexposure object
   *
   * @return GstElement*
   */
  GstElement *getAutoexposure();
  /**
   * @brief activate the freeze
   *
   * @param freeze
   */
  GstElement *getMultifocus();
  GstElement *getImageFreeze();
  void freezeStream(bool freeze);

  void getVideoSize(int *width, int *height);

  void switchToColor();
  void switchToGRAY();

private:
  /**
   * @brief Create a Elements object
   *
   */
  void createElements();
  /**
   * @brief link elements togethers
   *
   */
  void linkElementsGRAY();

private:
  bool color = false;
  /**
   * @brief pipeline
   *
   */
  /**
   * @brief video sources (nvarguscamerasrc in our case)
   *
   */
  GstElement *videosrc = nullptr;

  /**
   * @brief image freeze plugin
   *
   */
  GstElement *imageFreeze = nullptr;

  /**
   * @brief nvvidconv
   *
   */
  GstElement *nvvidconv = nullptr;
  GstElement *barcodereader = nullptr;
  GstElement *queue2 = nullptr;
  GstElement *autofocus = nullptr;

  GstElement *autoexposure = nullptr;
  GstElement *sharpness = nullptr;
  GstElement *multifocus = nullptr;


  GstElement *capsfilter = nullptr;

  GstPad *blockpad = nullptr;

};
static  GstElement *pipeline = nullptr;
 static GstElement *appsink = nullptr;
 static GstElement *whitebalance = nullptr;
 static GstElement *bayer2rgb = nullptr;
 static GstElement *gray2bayer = nullptr;
 static   GstElement *fpscounter = nullptr;  
 static GstElement *queue1 = nullptr;
