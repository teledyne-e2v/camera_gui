#include "Autofocus.hpp"
#include <stdio.h>
#include "utils.hpp"
#include <sstream>

typedef enum
{
    PENDING,
    IN_PROGRESS,
    WAITING,
    COMPLETED
} AutofocusStatus;

AutofocusControl::AutofocusControl(GstElement *autofocus, ModuleCtrl *moduleCtrl, ModuleControl *moduleControl, Config *conf, ROI *Roi)
    : autofocus(autofocus), moduleCtrl(moduleCtrl), moduleControl(moduleControl), conf(conf), Roi(Roi)
{
}

AutofocusControl::~AutofocusControl()
{
}

void AutofocusControl::updatePluginROI()
{
#ifndef DEBUG_MODE
        ImVec4 roi = Roi->getROI();
        if(roi.w != previousRoi.w || roi.x != previousRoi.x || roi.y != previousRoi.y || roi.z != previousRoi.z)
        {

        g_object_set(G_OBJECT(autofocus),
                     "x", roi.x ,
                     "y", roi.y,
                     "width", roi.z - roi.x,
                     "height", roi.w- roi.y,
                     NULL);
    }

#endif
}

void AutofocusControl::setVideoSize(int videoWidth, int videoHeight)
{
    Roi->setVideoSize(videoWidth, videoHeight);
}

void AutofocusControl::render(ImDrawList *drawList, ImVec2 streamSize, ImVec2 streamPosition, ImVec2 windowSize, ImVec2 windowPosition)
{
    ImGui::Begin("Autofocus Control");

    if (ImGui::Button("Reset ROI", buttonSize))
    {
        Roi->resetROI();
    }

    if (ImGui::Button("Center ROI", buttonSize))
    {
        Roi->centerROI(ImVec2(0.5, 0.5));
    }

    ImGui::RadioButton("Select ROI", &ref, 0); // add button to change ROI
    ImGui::RadioButton("Move ROI", &ref, 1);   // add button to move ROI

    ImGui::Spacing();
    Roi->render();
    ImGui::NewLine();

    if (ref == 0)
    {
        Roi->selectROI(streamPosition, streamSize, windowSize, windowPosition);
    }
    else if (ref == 1)
    {
        Roi->moveROI(streamPosition, streamSize);
    }

    // Button to launch autofocus
    if (ImGui::Button("Autofocus", buttonSize) && streamSize.y != 0 && streamSize.x != 0)
    {
#ifndef DEBUG_MODE
        updatePluginROI();
        g_object_set(G_OBJECT(autofocus), "autofocusStatus", PENDING, NULL);

        startingAutofocus = true;
#endif
    }
    if (!startingAutofocus && isAutofocusDone)
        isAutofocusDone = false;
    if (startingAutofocus)
    {
#ifndef DEBUG_MODE
        AutofocusStatus autofocusState;
        g_object_get(G_OBJECT(autofocus), "autofocusStatus", &autofocusState, NULL);

        if (autofocusState == COMPLETED)
        {
            isAutofocusDone = true;
            int PdaRegValue;
            double PdaVoltageValue;
            moduleCtrl->read_VdacPda(&PdaRegValue, &PdaVoltageValue);
            moduleControl->setPDA(PdaRegValue);
            startingAutofocus = false;
        }
#endif
    }

    if (ImGui::Button("Calibrating", buttonSize))
    {
#ifndef DEBUG_MODE
        updatePluginROI();
        g_object_set(G_OBJECT(autofocus), "calibrating", true, NULL);
#endif
        calibrating = true;
    }

    ImGui::Spacing();

    bool focus_lost = 0;

#ifndef DEBUG_MODE
    g_object_get(G_OBJECT(autofocus), "focus_lost", &focus_lost, NULL);
#endif

    if (focus_lost)
        updatePluginROI();

    Roi->drawROI(drawList, streamPosition, streamSize, displayRedRectangle && focus_lost);

    ImGui::Checkbox("Display lost focus", &displayRedRectangle);

    if (calibrating)
    {
#ifndef DEBUG_MODE
        g_object_get(G_OBJECT(autofocus), "calibrating", &calibrationState, NULL);

        if (!calibrationState)
        {
            int offset = 0;
            g_object_get(G_OBJECT(autofocus), "offset", &offset, NULL);
            int PdaRegValue;
            double PdaVoltageValue;
            moduleCtrl->read_VdacPda(&PdaRegValue, &PdaVoltageValue);
            moduleControl->setPDA(PdaRegValue);
            conf->setOffset(offset);
            calibrating = false;
        }
#endif
    }

    long int sharpness = 0;
    int pda = 0;

#ifndef DEBUG_MODE
    double PdaVoltageValue = 0;

    g_object_get(G_OBJECT(autofocus), "sharpness", &sharpness, NULL);
    moduleCtrl->read_VdacPda(&pda, &PdaVoltageValue);
#endif

    ImGui::NewLine();
    ImGui::Text("Sharpness: %ld", sharpness);
    ImGui::Spacing();
    ImGui::Text("PDA: %d", pda);

    ImGui::End();
}