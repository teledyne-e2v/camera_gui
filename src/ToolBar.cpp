#include "ToolBar.hpp"
#include "imgui.h"
#include <stdio.h>
ToolBar::ToolBar(Pipeline *pipeline)
    : pipeline(pipeline)
{
    pipeline->switchToColor(pixels[selected_pixel].c_str());
    isColor = true;
}

void ToolBar::render()
{

    if(step == 1)
    {
        pipeline->switchToGRAY();
        step = 2;
    }
    else if(step >=2)
    {
        if(step  > 5)
        {
        pipeline->switchToColor(pixels[selected_pixel].c_str());
        step = 0;
        }
        else
        {
            step++;
        }

    }
    ImGui::BeginMainMenuBar();
    if (!isColor)
    {
        if (ImGui::Button("Switch to color"))
        {
            pipeline->switchToColor(pixels[selected_pixel].c_str());
            isColor = true;
            step=0;
        }
    }
    else
    {
        if (ImGui::Button("Switch to monochrome"))
        {
            pipeline->switchToGRAY();
            isColor = false;
        }
    }
    ImGui::SetNextItemWidth(55.0);
    if (ImGui::BeginCombo("##debayers", pixels[selected_pixel].c_str()))
    {
        for (int i = 0; i < 4; i++)
        {
            if (ImGui::Selectable(pixels[i].c_str(), i == selected_pixel))
            {
                selected_pixel = i;
            }
        }
        ImGui::EndCombo();
    }
    if(previous_selected_pixel != selected_pixel)
    {

        //pipeline->switchToGRAY();
        //pipeline->switchToColor(pixels[selected_pixel].c_str());
        step=1;
        previous_selected_pixel  = selected_pixel;
    }
    ImGui::EndMainMenuBar();
}
