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

class Application
{
public:
    Application(int argc, char **argv);
    ~Application();

    void run();

private:
    void createFrame();
    void populateFrame();
    void renderFrame();

    void createDockSpace();
    void initImGuiConfig();

    void loadImGuiConfig();

private:
    Window *window;
    Pipeline * pipeline;

    ModuleCtrl *moduleCtrl;

    TakePhotos *photoTaker;
    ModuleControl *moduleControlConfig;
    Config *autofocusConfig;
    AutofocusControl *autofocusControl;
    Debug *autofocusDebug;
    BarcodeReader *barcodeReaderConfig;
    BarcodeDisplayer *barcodeDisplayer;

    GstMapInfo map;
    GLuint videotex;

    ImGuiDockNodeFlags dockspace_flags;
    ImGuiWindowFlags window_flags;

    int display_w = 0;
    int display_h = 0;

    int videoWidth = 0;
    int videoHeight = 0;

    bool frozen = false;
};