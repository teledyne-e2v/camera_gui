#pragma once

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/app/app.h>

#include "imgui.h"

#include "roi.hpp"
#include "ModuleControl_v100.hpp"
#include "moduleControl.hpp"
#include "config.hpp"

class AutofocusControl
{
public:
    AutofocusControl(GstElement *autofocus, ModuleCtrl *moduleCtrl, ModuleControl *moduleControl, Config *conf, ROI *Roi);
    ~AutofocusControl();

    void render(ImDrawList *drawList, ImVec2 streamSize, ImVec2 positionOfStream, ImVec2 windowSize, ImVec2 windowPosition);

    void setVideoSize(int videoWidth, int videoHeight);

public:
    bool isAutofocusDone = false;

private:
    ROI *Roi;

    GstElement *autofocus = nullptr;
    
    int ref = 0;
    bool displayRedRectangle = false;

    bool startingAutofocus = false;
    ModuleCtrl *moduleCtrl;
    ModuleControl *moduleControl;
    bool calibrating = false;
    bool calibrationState = false;
    Config *conf;

    const ImVec2 buttonSize = ImVec2(100, 0);
};
