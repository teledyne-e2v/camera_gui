#include "ToolBar.hpp"
#include "imgui.h"
#include <stdio.h>
ToolBar::ToolBar(Pipeline *pipeline) 
    :pipeline(pipeline)
{

}

void ToolBar::render()
{

    ImGui::BeginMainMenuBar();
    if(!isColor)
    {
        if(ImGui::Button("Switch to color"))
        {
            pipeline->switchToColor();
            isColor=true;
        }
    }
    else
    {
        if(ImGui::Button("Switch to monochrome"))
        {
            pipeline->switchToGRAY();
            isColor=false;
        }
    }
    ImGui::EndMainMenuBar();
}

