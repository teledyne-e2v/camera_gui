#pragma once

#include <gst/gst.h>
#include <string>
#include <vector>

#include "utils.hpp"

#define BarcodesNumber 16

struct COORDS
{
    ImVec2 point1;
    ImVec2 point2;
    ImVec2 point3;
    ImVec2 point4;
};

int lenOfString(const gchar *seq);

struct barcode
{
    std::string name;
    bool activated;
};

class BarcodeReader
{
public:
    BarcodeReader(GstElement *barcodereader);
    void render(ImDrawList *drawList, ImVec2 sizeOfStream, ImVec2 streamInWindow);

private:
    void apply();
    void getCoordinates();
    std::vector<COORDS> vectPoints;
    void frameBarcodes(ImDrawList *drawList,ImVec2 sizeOfStream ,ImVec2 streamInWindow);
    int getPos(const char * str,const size_t len, unsigned int i,const char delimiter);

private:
    GstElement *barcodereader;

    int number_barcode = 1;
    bool compression = false;

    barcode listOfBarcodes[BarcodesNumber];
    
    bool any = false;
    bool barcode1D = false;
    bool barcode2D = false;
    bool uncheckAll1D = false;
    bool uncheckAll2D = false;
    bool confChanged = false;
};
