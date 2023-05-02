#pragma once

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/app/app.h>
#include "roi.hpp"
#include "imgui.h"

class SharpnessControl
{
public:
    SharpnessControl(GstElement *sharpness, ROI *Roi);
    ~SharpnessControl();
    void render();

private:
    GstElement *sharpness = nullptr;
    bool work = false;
    bool toggleOnce = false;
    bool autodetect = true;
    bool previous_autodetect = false;
    int latency = 3;
    int previous_latency = 3;
    int number_of_plans = 3;
    int previous_number_of_plans = 4;
    int space_between_switch = 5;
    int previous_space_between_switch = 4;
    int plan1 = 0;
    int plan2 = 0;
    int plan3 = 0;
    int previous_plan1 = 0;
    int previous_plan2 = 0;
    int previous_plan3 = 0;

    ROI *Roi;
    void apply_ROI();
};
