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
    AutoexposureControl(GstElement *autoexposure,ModuleControl *moduleCtrl,ROI *Roi);
    ~AutoexposureControl();
    void apply_ROI();
    void render();
private:
    ROI* Roi;
    
    GstElement *autoexposure = nullptr;
    bool work=false;
    bool toggleOnce=true;
    bool toggleOnce3=true;
    bool useExpTime=false;
    bool useDigitalGain=true;
    bool debug=false;
    int min_digital_gain=256;
    int previous_min_digital_gain=256;
    int max_digital_gain=4096;
    int previous_max_digital_gain=4096;
    int max_analog_gain=15;
    int previous_max_analog_gain=15;
    int optimize=1;
    int previous_optimize=1;
    int latency=4;
    int previous_latency=4;
    int max_exp=20000;
    int previous_max_exp=20000;
    int target=60;
    int previous_target=60;
    bool loadAndSaveConf=true;
bool toggleOnce4=true;
bool toggleOnce5=true;
    ModuleControl *moduleControl;
	bool toggleOnce2=true;
    ImVec4 previousRoi;
};
