#include "Sharpness.hpp"
#include "utils.hpp"
SharpnessControl::SharpnessControl(GstElement *sharpness, ROI *Roi)
    : sharpness(sharpness), Roi(Roi)
{
}

void SharpnessControl::apply_ROI()
{
    ImVec4 roi = Roi->getROI();

    g_object_set(G_OBJECT(sharpness),
                 "roi1x", (int)roi.x,
                 "roi1y", (int)roi.y,
                 "roi2x", (int)roi.z,
                 "roi2y", (int)roi.w,
                 NULL);
}

SharpnessControl::~SharpnessControl()
{
}
void SharpnessControl::render()
{
    ImGui::Begin("Sharpness Control");
    ImGui::Text("Toggle sharpness viewer");
    ImGui::SameLine();
    if (ImGui::Checkbox("##Toggle sharpness", &work))
    {
        if (toggleOnce == false)
        {
            g_object_set(G_OBJECT(sharpness), "work", work, NULL);
            toggleOnce = true;
        }
    }
    else if (toggleOnce == true)
    {
        g_object_set(G_OBJECT(sharpness), "work", work, NULL);
        toggleOnce = false;
    }


    if (ImGui::Button("Reset sharpness algorithm"))
    {
        apply_ROI();
        g_object_set(G_OBJECT(sharpness), "reset", true, NULL);
    }

    ImGui::Text("Latency");
    ImGui::SameLine();
    ImGui::InputInt("##latency sharpness", &latency, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    if (latency != previous_latency)
    {
        g_object_set(G_OBJECT(sharpness), "latency", latency, NULL);
        previous_latency = latency;
    }

    ImGui::End();
}
