#pragma once

#include "Window.hpp"
#include "Pipeline.hpp"

#include "moduleControl.hpp"
#include "ModuleControl_v100.hpp"

#include "config.hpp"
#include "imgui_internal.h"
#include "Autofocus.hpp"

#include "takePhotos.hpp"
#include "Barcode.hpp"
#include "debug.hpp"
#include "BarcodeDisplayer.hpp"
#include "Autoexposure.hpp"
#include "Multifocus.hpp"
#include "Sharpness.hpp"
#include "ToolBar.hpp"
#include <vector>

#include <chrono>
#include <gst/gst.h>
class Application
{
public:
    Application(int argc, char **argv);
    ~Application();

    void run();

private:
    bool createFrame();
    void populateFrame();
    void renderFrame();

    void createDockSpace();
    void initImGuiConfig();

    void loadImGuiConfig();

private:
    Window *window;
    Pipeline *pipeline;


    GstBuffer *videobuf;

    ModuleCtrl *moduleCtrl;

    TakePhotos *photoTaker;
    ModuleControl *moduleControlConfig;
    Config *autofocusConfig;
    AutofocusControl *autofocusControl;
    ROI *Roi;
    Debug *autofocusDebug;
    BarcodeReader *barcodeReaderConfig;
    BarcodeDisplayer *barcodeDisplayer;
    AutoexposureControl *autoexposureControl;
    MultifocusControl *multifocusControl;
    SharpnessControl *sharpnessControl;
    ToolBar *toolbar;

    GstMapInfo map;
    GLuint videotex;
    
    GstElement *autofocus;
    GstElement *sharpness;
    GstElement *barcodereader;
    GstElement *freeze;
    GstElement *multifocus;
    GstElement *autoexposure;
    ImGuiDockNodeFlags dockspace_flags;
    ImGuiWindowFlags window_flags;
    
    int display_w = 0;
    int display_h = 0;
    std::vector<GstBuffer *> bufferNotFree;
    std::vector<GstMapInfo> mapNotFree;

    int videoWidth = 0;
    int videoHeight = 0;
    bool focus_lost=false;
    bool frozen = false;
    bool isBufferFree = 1;

    int frameCounter=0;
    std::chrono::_V2::system_clock::time_point start;
    std::chrono::_V2::system_clock::time_point end;
    float FPS=0;
};
