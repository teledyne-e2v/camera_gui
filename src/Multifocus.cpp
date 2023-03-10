#include "Multifocus.hpp"
#include "utils.hpp"
MultifocusControl::MultifocusControl(GstElement *multifocus,ROI *Roi)
:   multifocus(multifocus),Roi(Roi)
{

}


void MultifocusControl::apply_ROI()
{
    ImVec4 roi=Roi->getROI();

    g_object_set(G_OBJECT(multifocus),
                     "roi1x",(int) roi.x,
                     "roi1y",(int) roi.y,
                     "roi2x", (int)roi.z,
                     "roi2y",(int) roi.w,
                     NULL);
}

MultifocusControl::~MultifocusControl()
{
     
}
void MultifocusControl::render()
{
    ImGui::Begin("Multifocus Control");
	    ImGui::Text("Toggle multifocus");
    ImGui::SameLine();
    if (ImGui::Checkbox("##Toggle multifocus", &work))
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


ImGui::Text("Autodetect plans");
    ImGui::SameLine();
    if (ImGui::Checkbox("##Autodetect plans", &autodetect))
    {
        if (previous_autodetect == false)
        {
            g_object_set(G_OBJECT(multifocus), "auto_detect_plans", autodetect, NULL);
            previous_autodetect = true;
        }
    }
    else if (previous_autodetect == true)
    {
        g_object_set(G_OBJECT(multifocus), "auto_detect_plans", autodetect, NULL);
        previous_autodetect = false;
    }


    if(ImGui::Button("Next"))
{
    apply_ROI();
        g_object_set(G_OBJECT(multifocus), "next", true, NULL);
}

    if(ImGui::Button("Reset multifocus plans"))
{
    apply_ROI();
        g_object_set(G_OBJECT(multifocus), "reset", true, NULL);
}

    ImGui::Text("Latency");
    ImGui::SameLine();
    ImGui::InputInt("##latency multifocus", &latency, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    if (latency != previous_latency)
    {
        g_object_set(G_OBJECT(multifocus), "latency", latency, NULL);
        previous_latency = latency;
    }

    ImGui::Text("Number of plans");
    ImGui::SameLine();
    ImGui::InputInt("##number_of_plans multifocus", &number_of_plans, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    if (number_of_plans != previous_number_of_plans)
    {
        g_object_set(G_OBJECT(multifocus), "number_of_plans", number_of_plans, NULL);
        previous_number_of_plans = number_of_plans;
    }


    ImGui::Text("Number of frames between switches");
    ImGui::SameLine();
    ImGui::InputInt("##space_between_switch multifocus", &space_between_switch, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    if (space_between_switch != previous_space_between_switch)
    {
        g_object_set(G_OBJECT(multifocus), "space_between_switch", space_between_switch, NULL);
        previous_space_between_switch = space_between_switch;
    }
    

    if(ImGui::Button("Refresh"))
    {
        g_object_get(G_OBJECT(multifocus), "plan1", &plan1, NULL);
        previous_plan1 = plan1;
        g_object_get(G_OBJECT(multifocus), "plan2", &plan2, NULL);
        previous_plan1 = plan2;
        g_object_get(G_OBJECT(multifocus), "plan3", &plan3, NULL);
        previous_plan1 = plan3;
    }
    

    ImGui::Text("plan1");
    ImGui::SameLine();
    ImGui::InputInt("##plan1", &plan1, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    limit(plan1,-90,780);
    if (plan1 != previous_plan1)
    {
        g_object_set(G_OBJECT(multifocus), "plan1", plan1, NULL);
        previous_plan1 = plan1;
    }


    ImGui::Text("plan2");
    ImGui::SameLine();
    ImGui::InputInt("##plan2", &plan2, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    limit(plan2,-90,780);
    if (plan2 != previous_plan2)
    {
        g_object_set(G_OBJECT(multifocus), "plan2", plan2, NULL);
        previous_plan2 = plan2;
    }


    ImGui::Text("plan3");
    ImGui::SameLine();
    ImGui::InputInt("##plan3", &plan3, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    limit(plan3,-90,780);
    if (plan3 != previous_plan3)
    {
        g_object_set(G_OBJECT(multifocus), "plan3", plan3, NULL);
        previous_plan3 = plan3;
    }

ImGui::End();

}
