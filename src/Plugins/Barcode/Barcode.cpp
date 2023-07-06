#include "Barcode.hpp"
#include "utils.hpp"
#include <string.h>

BarcodeReader::BarcodeReader(GstElement *barcodereader)
{
    listOfBarcodes[0].name = "Aztec";
    listOfBarcodes[1].name = "Codabar";
    listOfBarcodes[2].name = "Code39";
    listOfBarcodes[3].name = "Code93";
    listOfBarcodes[4].name = "Code128";
    listOfBarcodes[5].name = "DataBar";
    listOfBarcodes[6].name = "DataBarExpanded";
    listOfBarcodes[7].name = "DataMatrix";
    listOfBarcodes[8].name = "EAN-8";
    listOfBarcodes[9].name = "EAN-13";
    listOfBarcodes[10].name = "ITF";
    listOfBarcodes[11].name = "MaxiCode";
    listOfBarcodes[12].name = "PDF417";
    listOfBarcodes[13].name = "QRCode";
    listOfBarcodes[14].name = "UPC-A";
    listOfBarcodes[15].name = "UPC-E";

    for (int i = 0; i < BarcodesNumber; i++)
    {
        listOfBarcodes[i].activated = false;
    }

#ifndef DEBUG_MODE

    std::string type;

    g_object_get(G_OBJECT(barcodereader),
                 "type", &type,
                 "compressed", &compression,
                 "number_barcode", &number_barcode,
                 NULL);

    int i = 0;
    while (type[i] != '\0')
    {
        int len = lenOfString(type.c_str());
        for (int j = 0; j < BarcodesNumber; j++)
        {
            if (strncmp(type.c_str() + i, listOfBarcodes[j].name.c_str(), len) == 0)
            {
                listOfBarcodes[j].activated = true;
                break;
            }
        }
        i += len;
        if (type[i] == ',')
        {
            i++;
        }
    }
#endif
    this->barcodereader = barcodereader;
}
// ; separate points, / separate barcodes, coordinates separated like x1,y1;x2,y2;x3,y3;x4,y4/x1,y1;x2,y2;x3,y3;x4,y4\0

int BarcodeReader::getPos(const char *str, const size_t len, unsigned int i, const char delimiter)
{
    while (i < len && str[i] != delimiter)
    {
        i++;
    }
    return i;
}

void BarcodeReader::getCoordinates()
{
    char *get_positions;
    g_object_get(G_OBJECT(barcodereader), "strPos", &get_positions, NULL);

    if (get_positions != NULL)
    {
        int len = strlen(get_positions);
        vectPoints.clear();
        int i = 0;
        while (i < len - 1)
        {
            COORDS points;
            int j = i;
            j = getPos(get_positions, len, j, ',');

            get_positions[j] = 0;
            int x = atoi(get_positions + i);
            i = j + 1;
            j = i;
            j = getPos(get_positions, len, j, ';');

            get_positions[j] = 0;
            int y = atoi(get_positions + i);
            points.point1.x = y;
            points.point1.y = x;
            i = j + 1;

            j = i;
            j = getPos(get_positions, len, j, ',');
            get_positions[j] = 0;
            x = atoi(get_positions + i);
            i = j + 1;
            j = i;
            j = getPos(get_positions, len, j, ';');
            get_positions[j] = 0;
            y = atoi(get_positions + i);
            points.point2.x = y;
            points.point2.y = x;
            i = j + 1;

            j = i;
            j = getPos(get_positions, len, j, ',');

            get_positions[j] = 0;
            x = atoi(get_positions + i);
            i = j + 1;
            j = i;
            j = getPos(get_positions, len, j, ';');
            get_positions[j] = 0;
            y = atoi(get_positions + i);
            points.point3.x = y;
            points.point3.y = x;
            i = j + 1;

            j = i;
            j = getPos(get_positions, len, j, ',');
            get_positions[j] = 0;
            x = atoi(get_positions + i);
            i = j + 1;
            j = i;
            j = getPos(get_positions, len, j, '/');
            get_positions[j] = 0;
            y = atoi(get_positions + i);
            points.point4.x = y;
            points.point4.y = x;
            i = j + 1;
            vectPoints.push_back(points);
        }
        free(get_positions);
    }
    else
    {
        vectPoints.clear();
    }
}

int lenOfString(const gchar *seq)
{
    int i = 0;

    while (seq[i] != ',' && seq[i] != 0)
    {
        i++;
    }

    return i;
}

void BarcodeReader::render(ImDrawList *drawList, ImVec2 sizeOfStream, ImVec2 streamInWindow)
{
    ImGui::Begin("Barcode Config");

    ImGui::PushItemWidth(-1);
    ImGui::Text("Number of barcodes");
    ImGui::SameLine();

    if (ImGui::InputInt("##number_barcode", &number_barcode, 1, 5, ImGuiInputTextFlags_CharsDecimal))
    {
#ifndef DEBUG_MODE
        g_object_set(G_OBJECT(barcodereader), "number_barcode", number_barcode, NULL);
#endif
    }

    ImGui::PopItemWidth();

    limit(number_barcode, 1, 10);

    ImGui::Text("Compression of the image");
    ImGui::SameLine();

    if (ImGui::Checkbox("##compression", &compression))
    {
#ifndef DEBUG_MODE
        g_object_set(G_OBJECT(barcodereader), "compressed", compression, NULL);
#endif
    }

    ImGui::NewLine();

    ImGui::Text("Types of barcodes");
    ImGui::Spacing();

    if (ImGui::Checkbox("Any barcodes", &any))
    {
        uncheckAll1D = !any;
        uncheckAll2D = !any;

        barcode1D = any;
        barcode2D = any;

        confChanged = true;
    }
    ImGui::NewLine();

    if (ImGui::CollapsingHeader("1D barcodes"))
    {
        if (ImGui::Checkbox("All 1D", &barcode1D))
        {
            uncheckAll1D = !barcode1D;
            confChanged = true;
        }

        ImGui::Spacing();

        bool tmpBarcode1D = true;

        for (int i : {1, 2, 3, 4, 5, 6, 8, 9, 10, 14, 15})
        {
            if (uncheckAll1D)
            {
                listOfBarcodes[i].activated = false;
            }
            else
            {
                listOfBarcodes[i].activated |= (any | barcode1D);
            }

            confChanged |= ImGui::Checkbox(listOfBarcodes[i].name.c_str(), &listOfBarcodes[i].activated);

            tmpBarcode1D &= listOfBarcodes[i].activated;
        }

        barcode1D = tmpBarcode1D;
        uncheckAll1D = false;
    }

    ImGui::Spacing();

    if (ImGui::CollapsingHeader("2D barcodes"))
    {
        if (ImGui::Checkbox("All 2D", &barcode2D))
        {
            uncheckAll2D = !barcode2D;
            confChanged = true;
        }

        ImGui::Spacing();

        bool tmpBarcode2D = true;

        for (int i : {0, 7, 11, 12, 13})
        {
            if (uncheckAll2D)
            {
                listOfBarcodes[i].activated = false;
            }
            else
            {
                listOfBarcodes[i].activated |= (any | barcode2D);
            }

            confChanged |= ImGui::Checkbox(listOfBarcodes[i].name.c_str(), &listOfBarcodes[i].activated);

            tmpBarcode2D &= listOfBarcodes[i].activated;
        }

        barcode2D = tmpBarcode2D;
        uncheckAll2D = false;
    }

    any = (barcode1D & barcode2D);

#ifndef DEBUG_MODE
    getCoordinates();
    frameBarcodes(drawList, sizeOfStream, streamInWindow);
#endif

    if (confChanged)
    {
        apply();
        confChanged = false;
    }

    ImGui::End();
}

void BarcodeReader::frameBarcodes(ImDrawList *drawList, ImVec2 sizeOfStream, ImVec2 streamInWindow)
{
    for (auto pos : vectPoints)
    {
        ImColor col = ImColor(ImVec4(0.1f, 0.4f, 1.0f, 1.0f));
        ImVec2 coef = sizeOfStream / ImVec2(1920, 1080);
        drawList->AddQuad(streamInWindow + coef * pos.point1, streamInWindow + coef * pos.point2, streamInWindow + coef * pos.point3, streamInWindow + coef * pos.point4, col, 3.0);
    }
}

void BarcodeReader::apply()
{
    std::string types = "";

    for (int i : {1, 2, 3, 4, 5, 6, 8, 9, 10, 14, 15})
    {
        // The status needs to be updated manually if the category is collapsed
        if (uncheckAll1D)
            listOfBarcodes[i].activated = false;
        else
            listOfBarcodes[i].activated |= any | barcode1D;

        if (listOfBarcodes[i].activated)
        {
            types += listOfBarcodes[i].name + ",";
        }
    }

    for (int i : {0, 7, 11, 12, 13})
    {
        // The status needs to be updated manually if the category is collapsed
        if (uncheckAll2D)
            listOfBarcodes[i].activated = false;
        else
            listOfBarcodes[i].activated |= any | barcode2D;

        if (listOfBarcodes[i].activated)
        {
            types += listOfBarcodes[i].name + ",";
        }
    }

    uncheckAll1D = false;
    uncheckAll2D = false;

    if (types.length() == 0)
    {
        types = "None";
    }
    else
    {
        types.pop_back();
    }

#ifndef DEBUG_MODE
    g_object_set(G_OBJECT(barcodereader), "type", types.c_str(), NULL);
#endif
}
