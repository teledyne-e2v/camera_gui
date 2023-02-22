#include "Autoexposure.hpp"

AutoexposureControl::AutofocusControl(GstElement *autoexposure,ModuleControl *moduleCtrl)
{
    Autoexposure=autoexposure;
    moduleControl=moduleCtrl;
}
AutoexposureControl::~AutofocusControl()
{

}
void AutoexposureControl::render()
{
    ImGui::Begin("Autoexposure Control");

    if (ImGui::Checkbox("Toggle autoexposure", work))
    {
        if(toggleOnce==false)
        {
            g_object_set(G_OBJECT(Autoexposure), "work", work, NULL);
            toggleOnce=true;
        }
        moduleControl->update_auto_controls();
    }
    else if(toggleOnce==true)
    {
        g_object_set(G_OBJECT(Autoexposure), "work", work, NULL);
        toggleOnce=false;
    }
    ImGui::End();
}
