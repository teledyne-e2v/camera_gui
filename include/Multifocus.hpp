#pragma once

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/app/app.h>
#include "roi.hpp"
#include "imgui.h"


class MultifocusControl
{
public:
    MultifocusControl(GstElement *multifocus,ROI *Roi);
    ~MultifocusControl();
    void render();
private:

    GstElement *multifocus = nullptr;
    bool work=false;
    bool toggleOnce=false;
    int latency=3;
    int previous_latency=3;
    int number_of_plans=3;
    int previous_number_of_plans=3;
    int space_between_switch;
    int previous_space_between_switch;
    ROI *Roi;
    void apply_ROI();
};
