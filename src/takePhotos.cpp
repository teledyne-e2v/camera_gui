#include "takePhotos.hpp"
#include <thread>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <unistd.h>
#include "utils.hpp"

bool showError = false;

TakePhotos::TakePhotos(GstMapInfo *map)
{
    this->map = map;

    realpath(getenv("HOME"), path);
}

void TakePhotos::render()
{
    float elementOffset = 140;

    ImGui::Begin("Take Photo");

    ImGui::PushItemWidth(-1);
    ImGui::Text("Number of photos:");
    ImGui::SameLine(elementOffset);
    ImGui::InputInt("##Number of Photos", &numberOfPhotos, 1, 10, ImGuiInputTextFlags_CharsDecimal);

    limit(numberOfPhotos, 1, 100);

    ImGui::Text("Delay:");
    ImGui::SameLine();
    ImGui::Button("i");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted("Delay in frame between each photo");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
    ImGui::SameLine(elementOffset);
    ImGui::InputInt("##delay", &frameOffset, 1, 100, ImGuiInputTextFlags_CharsDecimal);

    limit(frameOffset, 1, 1000);

    ImGui::Text("Path:");
    ImGui::SameLine(elementOffset);
    ImGui::InputText("##Photo path", path, 512);

    ImGui::Text("Name:");
    ImGui::SameLine(elementOffset);
    ImGui::InputText("##Photo name", photoName, 20);
    ImGui::PopItemWidth();

    ImGui::NewLine();

    if (takingPhotos)
        ImGui::BeginDisabled();

    if (ImGui::Button("Take photos"))
    {
        takingPhotos = true;
        frameCount = 0;
    }

    if (takingPhotos)
    {
        if (frameCount != 0)
            ImGui::EndDisabled();

        TakePhotosInMemory();
    }

    if (showError)
    {
        ImGui::NewLine();
        ImGui::PushStyleColor(ImGuiCol(0.9), ImVec4(1.0f, 0.25f, 0.0f, 1.0f));
        ImGui::Text("Error: Unable save at least one photo");
        ImGui::PopStyleColor();
    }

    ImGui::End();
}

unsigned char *TakePhotos::TakePhotoInMemory()
{
    unsigned char *frame = (unsigned char *)malloc(sizeof(unsigned char) * map->size);
    memcpy(frame, map->data, map->size);
    return frame;
}

void TakePhotos::TakePhotosInMemory()
{
    if (frameCount % frameOffset == 0)
    {
        unsigned char *frame = TakePhotoInMemory();
        frameVect.push_back(frame);
    }
    frameCount++;
    if ((int)frameVect.size() == numberOfPhotos) //  starts a new thread to save the frames.
    {
        std::thread savingFrameThread = std::thread(saveFrameFunction, &frameVect, imageWidth, imageHeight, path, photoName);
        savingFrameThread.detach();
        takingPhotos = false;
    }
}

void TakePhotos::setImageSize(int width, int height)
{
    imageWidth = width;
    imageHeight = height;
}


void addIFD(unsigned char *Y800data, int *offset, short int tag, short int fieldType, int count, int valueOffset)
{
    Y800data[*offset] = tag / 256;
    Y800data[*offset + 1] = tag % 256;
    Y800data[*offset + 2] = fieldType / 256;
    Y800data[*offset + 3] = fieldType % 256;
    Y800data[*offset + 4] = count >> 24;
    Y800data[*offset + 5] = (count >> 16) % 256;
    Y800data[*offset + 6] = (count >> 8) % (256 * 256);
    Y800data[*offset + 7] = count % (256 * 256 * 256);
    Y800data[*offset + 8] = valueOffset >> 24;
    Y800data[*offset + 9] = (valueOffset >> 16) % 256;
    Y800data[*offset + 10] = (valueOffset >> 8) % (256 * 256);
    Y800data[*offset + 11] = valueOffset % (256 * 256 * 256);
    *offset += 12;
}

void Y800toTIFF(unsigned char *Y800data, int row, int col, char *path, char *photoName, int photoNumber)
{
    unsigned char tiffImage[10 + 11 * 12];
    int offset = 8;
    tiffImage[0] = 0x4D;
    tiffImage[1] = 0x4D; // Big-endian
    tiffImage[2] = 0;
    tiffImage[3] = 42; // identifier of the file type
    tiffImage[4] = 0;  // offset
    tiffImage[5] = 0;
    tiffImage[6] = 0;
    tiffImage[7] = (unsigned char)offset;
    tiffImage[8] = 0;
    tiffImage[9] = (unsigned char)11; // IFD number

    offset += 2;
    addIFD(tiffImage, &offset, 256, 4, 1, col);           // image width
    addIFD(tiffImage, &offset, 257, 4, 1, row);           // image lenght
    addIFD(tiffImage, &offset, 258, 3, 1, 8 * 256 * 256); // BitsPerSample
    addIFD(tiffImage, &offset, 259, 3, 1, 1 * 256 * 256); // no compresion
    addIFD(tiffImage, &offset, 262, 3, 1, 1 * 256 * 256); // PhotometricInterpretation

    addIFD(tiffImage, &offset, 273, 4, 1, 10 + 11 * 12); // StripOffsets
    addIFD(tiffImage, &offset, 278, 4, 1, 2147483647);   // RowsPerStrip
    addIFD(tiffImage, &offset, 279, 4, 1, 1);            // StripByteCounts

    addIFD(tiffImage, &offset, 282, 5, 1, 0x696);         // XResolution
    addIFD(tiffImage, &offset, 283, 5, 1, 0x69E);         // YResolution
    addIFD(tiffImage, &offset, 296, 3, 1, 2 * 256 * 256); // ResolutionUnit

    std::ofstream flux;


    std::string pathStart = std::string(path) +
        ((path[strlen(path) - 1] == '/') ? "" : "/") +
        std::string(photoName) + "_" +
        std::to_string(photoNumber + 1);

    std::string fileName = pathStart + ".tiff";

    bool exist = true;
    int tag = 1;

    while (exist)
    {
        if (access(fileName.c_str(), 0) == 0)
        {
            fileName = pathStart + "(" + std::to_string(tag++) + ")" + ".tiff";
        }
        else
        {
            exist = false;
        }
    }

    flux.open(fileName);

    if (flux.is_open())
    {
        flux.write((char *)tiffImage, 10 + 11 * 12);
        flux.write((char *)Y800data, row * col);
        flux.close();
        showError = false;
    }
    else
    {
        showError = true;
    }
}

void saveFrameFunction(std::vector<unsigned char *> *frameVect, int width, int height, char *path, char *photoName)
{
    int i = 0;
    for (auto frame : *frameVect)
    {
        Y800toTIFF(frame, height, width, path, photoName, i);
        free(frame);
        i++;
    }
    frameVect->clear();
}

