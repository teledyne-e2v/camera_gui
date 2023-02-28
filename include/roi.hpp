#pragma once

#include <gst/gst.h>

#include "utils.hpp"
#include "imgui.h"

class ROI
{
public:
    ROI(GstElement *autofocus);
    ~ROI();

    /**
     * @brief Set the ROI to the size of the image
     *
     */
    void resetROI();

    /**
     * @brief Select the new ROI, using drag and drop
     *
     * @param windowPos
     * @param windowSize
     */
    void selectROI(ImVec2 streamPosition, ImVec2 streamSize, ImVec2 windowSize, ImVec2 windowPosition);

    /**
     * @brief Move the ROI with the mouse
     *
     * @param windowPos
     * @param windowSize
     */
    void moveROI(ImVec2 streamPosition, ImVec2 streamSize);

    /**
     * @brief Put the ROI at the center of the image
     *
     * @param width     The width of the ROI
     * @param height    The height of the ROI
     */
    void centerROI(ImVec2 center);

    /**
     * @brief Draw the ROI on the screen
     *
     * @param drawList
     * @param windowPos
     * @param windowSize
     * @param lostFocus
     */
    void drawROI(ImDrawList *drawList, ImVec2 windowPos, ImVec2 windowSize, bool lostFocus);

    /**
     * @brief Update the autofocus plugin properties related to the ROI
     *
     * @param element
     */
    void updatePluginROI();

    void render();

    void setVideoSize(int width, int height);

    ImVec4 getROI();

private:
    float videoWidth = 0;
    float videoHeight = 0;

    /**
     * @brief Whether or not the roi has been changed
     * 
     */
    bool changed = false;


    GstElement *autofocus;

    /**
     * @brief The normalized top left coordinates of the ROI (between (0, 0) and (1, 1))
     *
     */
    ImVec2 normPos1;

    /**
     * @brief The normalized bottom right coordinates of the ROI (between (0, 0) and (1, 1))
     *
     */
    ImVec2 normPos2;

    int lastSizeX;
    int lastSizeY;
    int centerX = 1920.0 / 2;
    int centerY = 1080.0 / 2;
    ImVec2 normDraggingPos;

    bool dragging = false;

    /**
     * @brief sort in increasing order
     *
     * @param p1
     * @param p2
     */
    void sort(float &p1, float &p2);
};
