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
    AutoexposureControl(GstElement *autoexposure);
    ~AutoexposureControl();
    void render();
private:

    GstElement *Autoexposure = nullptr;
    bool work=false;
    bool toggleOnce=true;
};
