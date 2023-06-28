#pragma once

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/app/app.h>
#include "roi.hpp"
#include "imgui.h"

class WhiteBalanceControl
{
public:
    WhiteBalanceControl(GstElement *whitebalance, ROI *Roi);
    ~WhiteBalanceControl();
    void render();
    void apply_ROI();

private:
    GstElement *whitebalance = nullptr;
    ROI *Roi;
    bool autowhitebalance=false;
    int red=0;
    int previous_red=0;
    int blue=0;
    int previous_blue=0;
    int green=0;
    int previous_green=0;
    bool auto_white_balance_toggled=true;
};
