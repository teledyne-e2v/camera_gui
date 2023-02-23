#include "Autoexposure.hpp"
#include "utils.hpp"
AutoexposureControl::AutoexposureControl(GstElement *autoexposure, ModuleControl *moduleCtrl,ROI *Roi)
    :   Roi(Roi),autoexposure(autoexposure)
{
    moduleControl = moduleCtrl;
}
AutoexposureControl::~AutoexposureControl()
{
}

void AutoexposureControl::apply_ROI()
{
    ImVec4 roi=Roi->getROI();

    g_object_set(G_OBJECT(autoexposure),
                     "ROI1x",(int) roi.x,
                     "ROI1y",(int) roi.y,
                     "ROI2x", (int)roi.z,
                     "ROI2y",(int) roi.w,
                     NULL);
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

    g_object_set(G_OBJECT(autoexposure), "useExpositionTime", useExpTime, NULL);
    toggleOnce2 = false;

    ImGui::Text("Optimization");
    ImGui::SameLine();
    ImGui::InputInt("Optimization", &optimize, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    if (optimize != previous_optimize)
    {
        g_object_set(G_OBJECT(autoexposure), "optimize", optimize, NULL);
        previous_optimize = optimize;
    }

    ImGui::Text("Maximum exposure");
    ImGui::SameLine();
    ImGui::InputInt("Maximum exposure", &max_exp, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    limit(max_exp,5,200000);
    if (max_exp != previous_max_exp)
    {
        g_object_set(G_OBJECT(autoexposure), "maxexposition", max_exp, NULL);
        previous_max_exp = max_exp;
    }

    ImGui::Text("latency");
    ImGui::SameLine();
    ImGui::InputInt("latency", &latency, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    limit(latency,0,100);
    if (latency != previous_latency)
    {
        g_object_set(G_OBJECT(autoexposure), "latency", latency, NULL);
        previous_latency = latency;
    }

    ImGui::Text("lowerbound");
    ImGui::SameLine();
    ImGui::InputInt("lowerbound", &lowerbound, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    limit(lowerbound,0,254);
    if (lowerbound != previous_lowerbound)
    {
        g_object_set(G_OBJECT(autoexposure), "lowerbound", lowerbound, NULL);
        previous_lowerbound = lowerbound;
    }

    ImGui::Text("upperbound");
    ImGui::SameLine();
    ImGui::InputInt("upperbound", &upperbound, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    limit(upperbound,1,255);
    if (upperbound != previous_upperbound)
    {
        g_object_set(G_OBJECT(autoexposure), "upperbound", upperbound, NULL);
        previous_upperbound = upperbound;
    }


	apply_ROI();
    moduleControl->update_auto_controls();
    ImGui::End();
}
