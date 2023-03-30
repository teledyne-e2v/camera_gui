#include "Autoexposure.hpp"
#include "utils.hpp"
AutoexposureControl::AutoexposureControl(GstElement *autoexposure, ModuleControl *moduleCtrl, ROI *Roi)
    : Roi(Roi), autoexposure(autoexposure)
{
    moduleControl = moduleCtrl;
}
AutoexposureControl::~AutoexposureControl()
{
}

void AutoexposureControl::apply_ROI()
{
    ImVec4 roi = Roi->getROI();
    if(roi != previousRoi)
    {
        previousRoi = roi;
        g_object_set(G_OBJECT(autoexposure),
                 "roi1x", (int)roi.x,
                 "roi1y", (int)roi.y,
                 "roi2x", (int)roi.z,
                 "roi2y", (int)roi.w,
                 NULL);
    }
}

void AutoexposureControl::render()
{
    ImGui::Begin("Autoexposure Control");

    if (ImGui::Checkbox("Toggle autoexposure", &work))
    {
        if (toggleOnce == false)
        {
            g_object_set(G_OBJECT(autoexposure), "work", work, NULL);
            toggleOnce = true;
        }
    }
    else if (toggleOnce == true)
    {
        g_object_set(G_OBJECT(autoexposure), "work", work, NULL);
        toggleOnce = false;
    }

    if (ImGui::Checkbox("Use exposition time", &useExpTime))
    {
        if (toggleOnce2 == false)
        {
            g_object_set(G_OBJECT(autoexposure), "useExpositionTime", useExpTime, NULL);
            toggleOnce2 = true;
        }
    }
    else if (toggleOnce2 == true)
    {
        g_object_set(G_OBJECT(autoexposure), "useExpositionTime", useExpTime, NULL);
        toggleOnce2 = false;
    }

    if (ImGui::Checkbox("Use Digital Gain", &useDigitalGain))
    {
        if (toggleOnce3 == false)
        {
            g_object_set(G_OBJECT(autoexposure), "useDigitalGain", useDigitalGain, NULL);
            toggleOnce3 = true;
        }
    }
    else if (toggleOnce3 == true)
    {
        g_object_set(G_OBJECT(autoexposure), "useDigitalGain", useDigitalGain, NULL);
        toggleOnce3 = false;
    }

    ImGui::Text("Optimization");
    ImGui::SameLine();
    ImGui::InputInt("##Optimization", &optimize, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    if (optimize != previous_optimize)
    {
        g_object_set(G_OBJECT(autoexposure), "optimize", optimize, NULL);
        previous_optimize = optimize;
    }

    ImGui::Text("Maximum exposure");
    ImGui::SameLine();
    ImGui::InputInt("##Maximum exposure", &max_exp, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    limit(max_exp, 5, 200000);
    if (max_exp != previous_max_exp)
    {
        g_object_set(G_OBJECT(autoexposure), "maxExposition", max_exp, NULL);
        previous_max_exp = max_exp;
    }

    ImGui::Text("Maximum analog gain");
    ImGui::SameLine();
    ImGui::InputInt("##Maximum analog gain", &max_analog_gain, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    limit(max_analog_gain,5,200000);
    if (max_analog_gain != previous_max_analog_gain)
    {
        g_object_set(G_OBJECT(autoexposure), "maxAnalogGain", max_analog_gain, NULL);
        previous_max_analog_gain = max_analog_gain;
    }

    ImGui::Text("latency");
    ImGui::SameLine();
    ImGui::InputInt("##latency", &latency, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    limit(latency, 0, 100);
    if (latency != previous_latency)
    {
        g_object_set(G_OBJECT(autoexposure), "latency", latency, NULL);
        previous_latency = latency;
    }

    ImGui::Text("target");
    ImGui::SameLine();
    ImGui::InputInt("##target", &target, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    limit(target, 0, 255);
    if (target != previous_target)
    {
        g_object_set(G_OBJECT(autoexposure), "target", target, NULL);
        previous_target = target;
    }

    apply_ROI();
    moduleControl->update_auto_controls();
    ImGui::End();
}
