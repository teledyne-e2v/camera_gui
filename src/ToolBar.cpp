#include "ToolBar.hpp"
#include "imgui.h"

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
        if(ImGui::Button("Switch to GRAY"))
        {
            pipeline->switchToGRAY();
            isColor=false;
        }
    }
    ImGui::EndMainMenuBar();

}