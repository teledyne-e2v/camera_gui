#pragma once

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/app/app.h>

#include "imgui.h"

#include "roi.hpp"
#include "ModuleControl_v100.hpp"
#include "moduleControl.hpp"
#include "config.hpp"

class AutoexposureControl
{
public:
    AutoexposureControl(GstElement *autoexposure,ModuleControl *moduleCtrl);
    ~AutoexposureControl();
    void render();
private:

    GstElement *Autoexposure = nullptr;
    bool work=false;
    bool toggleOnce=true;
    bool useExpTime=false;
    int optimize=1;
    int previous_optimize=1;
    int latency=4;
    int previous_latency=4;
    int max_exp=20000;
    int previous_max_exp=20000;
    int lowerbound=50;
    int previous_lowerbound=50;
    int upperbound=110;
    int previous_upperbound=110;
    ModuleControl *moduleControl;
	bool toggleOnce2=true;
};
