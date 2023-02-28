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
    MultifocusControl(GstElement *Multifocus,ModuleControl *moduleCtrl);
    ~MultifocusControl();
    void render();
private:

    GstElement *Multifocus = nullptr;
    bool work;
    bool toggleOnce=false;
    ModuleControl *moduleControl;
};
