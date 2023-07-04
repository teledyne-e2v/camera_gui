#pragma once

#include "imgui.h"
#include <gst/gst.h>
#include <vector>

/**
 * @brief
 *
 * @param frameVect
 * @param path
 * @param photoName
 */
void saveFrameFunction(std::vector<unsigned char *> *frameVect, int width, int height, char *path, char *photoName);

/**
 * @brief
 *
 */
class TakePhotos
{
public:
    /**
     * @brief render the window
     *
     */
    void render(bool frame_created);

    /**
     * @brief Construct a new Take Photos object
     *
     * @param map
     */
    TakePhotos(GstMapInfo *map);

    void setImageSize(int width, int height);

private:
    int imageWidth = 1920;
    int imageHeight = 1080;

    /**
     * @brief true when its already taking photos
     *
     */
    bool takingPhotos = false;
    /**
     * @brief number off photos which are going to be taken
     *
     */
    int numberOfPhotos = 1;
    /**
     * @brief number of frames between each photos
     *
     */
    int frameOffset = 1;

    GstMapInfo *map;
    /**
     * @brief Function that take all the photos in memory
     *
     */
    void TakePhotosInMemory();

    /**
     * @brief Function that take a single photo in memory
     *
     * @return unsigned char*
     */
    unsigned char *TakePhotoInMemory();

    /**
     * @brief counter of frames
     *
     */
    int frameCount;

    /**
     * @brief vector which contain all the photos
     *
     */
    std::vector<unsigned char *> frameVect;

    /**
     * @brief
     * name of the photos
     */
    char photoName[20] = "photos";

    /**
     * @brief path of the aimed directory
     *
     */
    char path[512];
};
