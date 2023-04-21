#include "Multifocus.hpp"
#include "utils.hpp"
#include <string>
#include <gst/gst.h>
MultifocusControl::MultifocusControl(GstElement *multifocus, ROI *Roi)
    : multifocus(multifocus), Roi(Roi)
{
	for(int i=0;i<50;i++)
	{
		plans[i]=0;
	}	
}

void MultifocusControl::apply_ROI()
{
    ImVec4 roi = Roi->getROI();

    g_object_set(G_OBJECT(multifocus),
                 "roi1x", (int)roi.x,
                 "roi1y", (int)roi.y,
                 "roi2x", (int)roi.z,
                 "roi2y", (int)roi.w,
                 NULL);
}

MultifocusControl::~MultifocusControl()
{
}


void parseString(char* string, int *tab,int size)
{
	for(int i=0;i<size;i++)
	{
		int size_of_number;
		char tmp[10];
		size_of_number = sscanf(string,"%d;",&(tab[i]));
		if(size_of_number<0)
			return;

		snprintf(tmp,10,"%d",tab[i]);
		string+=strlen(tmp)+1;


	}
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

    if (ImGui::Button("Next"))
    {
        apply_ROI();
        g_object_set(G_OBJECT(multifocus), "next", true, NULL);
    }

    if (ImGui::Button("Reset multifocus plans"))
    {
        apply_ROI();
	reset = true;
	previous_reset=true;
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
    limit(number_of_plans, 1, 50);
    ImGui::Text("Number of frames between switches");
    ImGui::SameLine();
    ImGui::InputInt("##space_between_switch multifocus", &space_between_switch, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    if (space_between_switch != previous_space_between_switch)
    {
        g_object_set(G_OBJECT(multifocus), "space_between_switch", space_between_switch, NULL);
        previous_space_between_switch = space_between_switch;
    }

    g_object_get(G_OBJECT(multifocus), "reset", &reset, NULL);
    if (reset != previous_reset)
    {
	char * tmpchar;
	g_object_get(G_OBJECT(multifocus), "plans", &tmpchar, NULL);
	g_object_get(G_OBJECT(multifocus), "number_of_plans", &number_of_plans, NULL);
	previous_number_of_plans = number_of_plans;
	previous_reset=reset;

	parseString(tmpchar, plans,number_of_plans);
	printf("char get : %s\n",tmpchar);


    }



    for(int i=0;i<number_of_plans;i++)
{

	std::string s = "plan";
	std::string s2 = "##";
	auto result = s + std::to_string(i);
	auto result2 = s2 + result;
    ImGui::Text(result.c_str());
    ImGui::SameLine();
    ImGui::InputInt(result2.c_str(), plans+i, 0, 1, ImGuiInputTextFlags_CharsDecimal);
    limit(plans[i], -90, 780);
	if(plans[i]!=previous_plans[i])
	{
		previous_plans[i]=plans[i];
		rewrite=true;
	}
    
   }
if(rewrite)
{
tmp="";
rewrite = false;
for(int i=0;i<number_of_plans;i++)
{
	tmp=tmp + std::to_string(plans[i]) + ";";

}
	printf("i'm in : %s\n", tmp.c_str());
    	g_object_set(G_OBJECT(multifocus), "plans", tmp.c_str(), NULL);
}


    ImGui::End();
}
