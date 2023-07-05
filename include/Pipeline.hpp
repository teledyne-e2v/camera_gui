#pragma once

#include <gst/gst.h>
#include <map>

struct Data{
  std::map<std::string,GstElement*> *GstElements;
  char pixels[5];
};

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
  Pipeline(int argc, char **argv, bool color, bool multifocus);
  ~Pipeline();


  GstElement *getSharpness();


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

  void switchToColor(const char * pixels);
  void switchToGRAY();
  bool getColorSupport() { return this->color_support; }

private:
  std::map<std::string,GstElement*> GstElements;
  struct Data data;

  /**
   * @brief Create a Elements object
   *
   */
  void createElements(bool color, bool multifocus);
  /**
   * @brief link elements togethers
   *
   */
  void linkElementsGRAY();

  void mapping_Gst_Element(std::string name);

  bool color_support;





  /**
   * @brief video sources (nvarguscamerasrc in our case)
   *
   */
  GstElement *videosrc = nullptr;


  /**
   * @brief nvvidconv
   *
   */
  GstElement *pipeline = nullptr;

  GstPad *blockpad = nullptr;
};

