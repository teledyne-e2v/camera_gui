#include "Autoexposure.hpp"

AutoexposureControl::AutoexposureControl(GstElement *autoexposure)
{
    Autoexposure=autoexposure;
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
    ImGui::End();
}
