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
    bool isAutofocusDone = false;

private:

	
    GstElement *autofocus = nullptr;

    bool displayRedRectangle = false;

    bool startingAutofocus = false;
    ModuleCtrl *moduleCtrl;
    ModuleControl *moduleControl;
	Config *conf = NULL;
    ROI *Roi = NULL;
    bool calibrating = false;
    bool sharpnessCalculation = false;
    bool previouSharpnessCalculation = false;

    bool calibrationState = false;
    ImVec4 previousRoi;
    void updatePluginROI();


    const ImVec2 buttonSize = ImVec2(100, 0);
public:
    AutofocusControl(GstElement *autofocus, ModuleCtrl *moduleCtrl, ModuleControl *moduleControl, Config *conf, ROI *Roi);
    ~AutofocusControl();

    bool render(ImDrawList *drawList, ImVec2 streamSize, ImVec2 positionOfStream, ImVec2 windowSize, ImVec2 windowPosition);
};
