#pragma once

#include <gst/gst.h>

/**
 * @brief Class creating and working on the pipeline
 * 
 */
class Pipeline
{
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

    /**
     * @brief activate the freeze
     * 
     * @param freeze 
     */
    void freezeStream(bool freeze);

    void getVideoSize(int *width, int *height);

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
    void linkElements();

private:
/**
 * @brief pipeline
 * 
 */
    GstElement *pipeline        = nullptr;
    /**
     * @brief video sources (nvarguscamerasrc in our case)
     * 
     */
    GstElement *videosrc        = nullptr;

    /**
     * @brief image freeze plugin
     * 
     */
    GstElement *imageFreeze     = nullptr;

    /**
     * @brief nvvidconv
     * 
     */
    GstElement *nvvidconv       = nullptr;
    GstElement *barcodereader   = nullptr;
    GstElement *queue           = nullptr;
    GstElement *autofocus       = nullptr;
    GstElement *appsink         = nullptr;
};