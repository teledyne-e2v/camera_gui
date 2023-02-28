#include "Multifocus.hpp"

MultifocusControl::MultifocusControl(GstElement *multifocus)
:   multifocus(multifocus)
{

}
MultifocusControl::~MultifocusControl()
{

}
void MultifocusControl::render()
{
    ImGui::Begin("Autoexposure Control");

    if (ImGui::Checkbox("Toggle multifocus", &work))
    {
        if (toggleOnce == false)
        {
            g_object_set(G_OBJECT(multifocus), "work", work, NULL);
            toggleOnce = true;
        }
    }
    else if (toggleOnce == true)
    {
        g_object_set(G_OBJECT(multifocus), "work", work, NULL);
        toggleOnce = false;
    }

    ImGui::Text("Latency");
    ImGui::SameLine();
    ImGui::InputInt("latency multifocus", &latency, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    if (latency != previous_latency)
    {
        g_object_set(G_OBJECT(autoexposure), "latency", latency, NULL);
        previous_latency = latency;
    }

    ImGui::Text("Number of plans");
    ImGui::SameLine();
    ImGui::InputInt("number_of_plans multifocus", &number_of_plans, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    if (number_of_plans != previous_number_of_plans)
    {
        g_object_set(G_OBJECT(autoexposure), "number_of_plans", number_of_plans, NULL);
        previous_number_of_plans = number_of_plans;
    }


    ImGui::Text("Number of frames between switches");
    ImGui::SameLine();
    ImGui::InputInt("space_between_switch multifocus", &space_between_switch, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    if (space_between_switch != previous_space_between_switch)
    {
        g_object_set(G_OBJECT(autoexposure), "space_between_switch", space_between_switch, NULL);
        previous_space_between_switch = space_between_switch;
    }
    ImGui::End();
}
