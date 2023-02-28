#pragma once

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/app/app.h>

#include "imgui.h"

#include "ModuleControl_v100.hpp"
#include "moduleControl.hpp"
#include "config.hpp"

class MultifocusControl
{
public:
    MultifocusControl(GstElement *multifocus);
    ~MultifocusControl();
    void render();
private:

    GstElement *multifocus = nullptr;
    bool work=false;
    bool toggleOnce=false;
    int latency;
    int previous_latency;
    int number_of_plans;
    int previous_number_of_plans;
    int space_between_switch;
    int previous_space_between_switch;
};
