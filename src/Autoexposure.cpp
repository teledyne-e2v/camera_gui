#include "Autoexposure.hpp"

AutoexposureControl::AutoexposureControl(GstElement *autoexposure,ModuleControl *moduleCtrl)
{
    Autoexposure=autoexposure;
    moduleControl=moduleCtrl;
}
AutoexposureControl::~AutoexposureControl()
{

}
void AutoexposureControl::render()
{
    ImGui::Begin("Autoexposure Control");

    if (ImGui::Checkbox("Toggle autoexposure", &work))
    {
        if(toggleOnce==false)
        {
            g_object_set(G_OBJECT(Autoexposure), "work", work, NULL);
            toggleOnce=true;
        }

    }
    else if(toggleOnce==true)
    {
        g_object_set(G_OBJECT(Autoexposure), "work", work, NULL);
        toggleOnce=false;
    }

    if (ImGui::Checkbox("Use exposition time", &useExpTime))
    {
	if(toggleOnce2==false)
        {
            g_object_set(G_OBJECT(Autoexposure), "useExpositionTime", useExpTime, NULL);
            toggleOnce2=true;
        }
    }
    else if(toggleOnce2==true)
    {
        g_object_set(G_OBJECT(Autoexposure), "useExpositionTime", useExpTime, NULL);
        toggleOnce2=false;
    }

    g_object_set(G_OBJECT(Autoexposure), "useExpositionTime", useExpTime, NULL);
    toggleOnce2=false;

    ImGui::Text("Optimization");
    ImGui::SameLine();
    ImGui::InputInt("Optimization", &optimize, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    if (optimize < 0)
        optimize = 0;
    else if (optimize > 5)
        optimize = 5;
    if(optimize!=previous_optimize)
{
g_object_set(G_OBJECT(Autoexposure), "optimize", optimize, NULL);
previous_optimize=optimize;
}

     ImGui::Text("Maximum exposure");
    ImGui::SameLine();
    ImGui::InputInt("Maximum exposure", &max_exp, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    if (max_exp < 0)
        max_exp = 0;
    else if (max_exp > 200000)
        max_exp = 200000;
    if(max_exp!=previous_max_exp)
{
g_object_set(G_OBJECT(Autoexposure), "maxexposition", max_exp, NULL);
previous_max_exp=max_exp;
}



    moduleControl->update_auto_controls();
    ImGui::End();
}
