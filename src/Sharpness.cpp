#include "Sharpness.hpp"
#include "utils.hpp"
#include "external/implot/implot.h"
#include <stdio.h>
SharpnessControl::SharpnessControl(GstElement *sharpness, ROI *Roi)
    : sharpness(sharpness), Roi(Roi)
{
    strncpy(filename,"result.csv",200);
}

void SharpnessControl::apply_ROI()
{
    ImVec4 roi = Roi->getROI();
    printf("ROI : %f %f %f %f \n", roi.x, roi.y, roi.z, roi.w);
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


    ImGui::Text("Step");
    ImGui::SameLine();
    ImGui::InputInt("##step sharpness", &step, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    if (step != previous_step)
    {
        g_object_set(G_OBJECT(sharpness), "step", step, NULL);
        previous_step = step;
    }



    ImGui::Text("Path:");
    ImGui::SameLine();
    ImGui::InputText("##Photo path", filename, 200);
    if(strncmp(filename,previous_filename,200))
    {
        strncpy(previous_filename,filename,200);
        g_object_set(G_OBJECT(sharpness), "filename", filename, NULL);
    }


    ImGui::End();

    g_object_get(G_OBJECT(sharpness), "done", &done, NULL);
    if (done != previous_done)
    {
        previous_done = done;
        if (done)
        {
            done_once = true;
            FILE *file;

            char *line = NULL;
            size_t len = 0;

            file = fopen(filename, "r+");
            getline(&line, &len, file);

            number_of_values = 0;
            while (!feof(file))
            {
                fscanf(file, "%d, %d\n", &(csv_x[number_of_values]), &(csv_y[number_of_values]));
                number_of_values++;
            }

            fclose(file);
        }
    }
    if (done_once)
    {

        ImGui::Begin("Plot");
        if (ImPlot::BeginPlot("My Plot"))
        {

            ImPlot::PlotLine("My Line Plot", csv_x, csv_y, number_of_values);
            ImPlot::EndPlot();
        }
        ImGui::End();
    }
}
