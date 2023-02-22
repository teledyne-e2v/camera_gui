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
    bool work;
    bool toggleOnce=false;
    ModuleControl *moduleControl;
};
