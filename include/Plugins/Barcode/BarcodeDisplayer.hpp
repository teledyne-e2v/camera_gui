
#pragma once
#include <gst/gst.h>
#include "imgui.h"
#include "utils.hpp"
class BarcodeDisplayer
{
    private:
        GstElement *barcodereader;
        char barcodes[80000];
        void updateLogs();
    public:
        BarcodeDisplayer(GstElement *barcodereader);
        void render();


};