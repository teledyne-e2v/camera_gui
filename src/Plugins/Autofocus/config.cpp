
#include "config.hpp"
#include <cstdio>

Config::Config(GstElement *autofocus)
{
#ifndef DEBUG_MODE
    this->autofocus = autofocus;
    g_object_get(G_OBJECT(autofocus),
                 "strategy", &strategy,
                 "step_small", &smallStep,
                 "step_big", &bigStep,
                 "pda_min", &pdaMin,
                 "pda_max", &pdaMax,
                 "dec", &dec,
                 "offset", &offset,
                 "continuous", &cont,
                 "continuous_update_interval", &continuous_update_interval,
                 "continuous_threshold", &continuous_threshold,
                 "continuous_timeout", &continuous_timeout,
                 "pda_hold_cmd", &pda_hold_cmd,
                 NULL);
#endif
}

void Config::apply()
{
    security();

#ifndef DEBUG_MODE
    if (previousOffset != offset)
        g_object_set(G_OBJECT(autofocus), "offset", offset, NULL);
    if (previous_continuous_timeout != continuous_timeout)
        g_object_set(G_OBJECT(autofocus), "continuous_timeout", continuous_timeout, NULL);
    if (previous_continuous_update_interval != continuous_update_interval)
        g_object_set(G_OBJECT(autofocus), "continuous_update_interval", continuous_update_interval, NULL);
    if (previousBigStep != bigStep)
        g_object_set(G_OBJECT(autofocus), "step_big", bigStep, NULL);
    if (previousSmallStep != smallStep)
        g_object_set(G_OBJECT(autofocus), "step_small", smallStep, NULL);
    if (previousPdaMin != pdaMin)
        g_object_set(G_OBJECT(autofocus), "pda_min", pdaMin, NULL);
    if (previousPdaMax != pdaMax)
        g_object_set(G_OBJECT(autofocus), "pda_max", pdaMax, NULL);
    if (previousDec != dec)
        g_object_set(G_OBJECT(autofocus), "dec", dec, NULL);
    if (previousCont != cont)
        g_object_set(G_OBJECT(autofocus), "continuous", cont, NULL);
    if (previous_contiuous_threshold != continuous_threshold)
        g_object_set(G_OBJECT(autofocus), "continuous_threshold", continuous_threshold, NULL);

    if (previous_pda_hold_cmd != pda_hold_cmd)
        g_object_set(G_OBJECT(autofocus), "pda_hold_cmd", pda_hold_cmd, NULL);
    if (previousStrategy != strategy)
        g_object_set(G_OBJECT(autofocus), "strategy", strategy, NULL);
#endif
    previousOffset = offset;
    previous_continuous_timeout = continuous_timeout;
    previous_continuous_update_interval = continuous_update_interval;
    previous_contiuous_threshold = continuous_threshold;
    previous_pda_hold_cmd = pda_hold_cmd;
    previousBigStep = bigStep;
    previousCont = cont;
    previousDec = dec;
    previousPdaMax = pdaMax;
    previousPdaMin = pdaMin;
    previousSmallStep = smallStep;
    previousStrategy = strategy;
}

void Config::render()
{
    if (showWindow)
    {
        float elementOffset = 180;

        ImGui::Begin("Autofocus Config");
        /**
         * Autofocus
         **/
        ImGui::Text("Autofocus :");

        ImGui::SameLine();
        ImGui::Button("i");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted("Choose the autofocus algorithm");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
        const char *items[nbAlgos];
        items[0] = "Naïve autofocus (more precise but slow)";
        items[1] = "Optimized autofocus";
	items[2] = "Weighted mean autofocus";
	items[3] = "Gaussian prediction autofocus";
        ImGui::PushItemWidth(-1);
        if (ImGui::BeginCombo("##Strategies", items[strategy]))
        {
            for (int i = 0; i < nbAlgos; i++)
            {
                if (ImGui::Selectable(items[i], i == strategy))
                {
                    strategy = i;
                }
            }
            ImGui::EndCombo();
        }

        ImGui::NewLine();



        /**
         * Small Step and Big Step
         **/

        ImGui::Text("Small Step:");
        ImGui::SameLine(elementOffset);
        ImGui::InputInt("##smallStep", &smallStep, 1, 10, ImGuiInputTextFlags_CharsDecimal);

        if (strategy == 0)
            ImGui::BeginDisabled();

        ImGui::Text("Big Step:");
        ImGui::SameLine(elementOffset);

        ImGui::InputInt("##bigStep", &bigStep, 1, 10, ImGuiInputTextFlags_CharsDecimal);

        if (strategy == 0)
            ImGui::EndDisabled();

        /**
         * PDA min
         **/
        ImGui::Text("PDA min:");
        ImGui::SameLine(elementOffset);
        ImGui::InputInt("##pdaMin", &pdaMin, 1, 10, ImGuiInputTextFlags_CharsDecimal);

        /**
         * PDA max
         **/
        ImGui::Text("PDA max:");
        ImGui::SameLine(elementOffset);
        ImGui::InputInt("##pdaMax", &pdaMax, 1, 10, ImGuiInputTextFlags_CharsDecimal);

        /**
         * Dec
         **/
        ImGui::Text("dec:");
        ImGui::SameLine();
        ImGui::Button("i");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted("Corresponds to the number of consecutive less sharp images we should have before stopping the autofocus algorithm.");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
        ImGui::SameLine(elementOffset);
        ImGui::InputInt("##dec", &dec, 1, 10, ImGuiInputTextFlags_CharsDecimal);

        /**
         * Offset
         **/
        ImGui::Text("Offset:");
        ImGui::SameLine();
        ImGui::Button("i");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted("Number of images to ignore before calculating the sharpness right after sending a pda command. It corresponds to the number of images between the moment we send a command to the lens and the moment we receive the image corresponding to the command.\nIt can be calibrated by clicking on the calibration button in the \"Autofocus Controle\" pannel.\nIt is also used for the step...");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
        ImGui::SameLine(elementOffset);
        ImGui::InputInt("##Offset", &offset, 1, 10, ImGuiInputTextFlags_CharsDecimal);

        /**
         * Pda Hold
         **/
        ImGui::Text("pda hold:");
        ImGui::SameLine();
        ImGui::Button("i");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted("Number of images during which the pda is hold before sending the pda command. If equal 0, it will be hold during the current frame and the command will be send at the end of the current frame. If equal 1, it will be hold during the current and the next frame. If equal n, it will be hold during the current frame and the n next frames.");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
        ImGui::SameLine(elementOffset);
        ImGui::InputInt("##pda_hold_cmd", &pda_hold_cmd, 1, 10, ImGuiInputTextFlags_CharsDecimal);

        items[0] = "False";
        items[1] = "True";

        ImGui::NewLine();
        ImGui::Text("Continuous autofocus:");
        ImGui::SameLine();
        ImGui::Button("i");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted("With this mode enabled, the autofocus we relaunch itself if it detects that the image is no longer sharp");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }

        ImGui::Checkbox("Enabled", &cont);

        ImGui::Spacing();

        if (!cont)
            ImGui::BeginDisabled();

        ImGui::Text("Update interval:");
        ImGui::SameLine(elementOffset);
        ImGui::InputInt("##interval", &continuous_update_interval, 1, 10, ImGuiInputTextFlags_CharsDecimal);

        ImGui::Text("Threshold:");
        ImGui::SameLine(elementOffset);
        ImGui::InputFloat("##continuous_threshold", &continuous_threshold, 5, 50, "%.2f %", ImGuiInputTextFlags_CharsDecimal);

        ImGui::Text("Time out:");
        ImGui::SameLine(elementOffset);
        ImGui::InputInt("##continuous_timeout", &continuous_timeout, 1, 10, ImGuiInputTextFlags_CharsDecimal);





        if (!cont)
            ImGui::EndDisabled();

        ImGui::PopItemWidth();

        apply();

        ImGui::End();
    }
}


int Config::getStrategy()
{
	return strategy;
}

void Config::security() // refer to the limit documentation of the autofocus plugin
{
    limit(strategy, 0, nbAlgos);
    limit(smallStep, 1, bigStep);
    limit(bigStep, smallStep, 700);
    limit(pdaMin, -90, pdaMax);
    limit(pdaMax, pdaMin, 750);
    limit(dec, 0, 20);
    limit(offset, 0, 100);
    limit(continuous_update_interval, 1, 120);
    limit(continuous_threshold, 1.0f, 100.0f);
    limit(continuous_timeout, 1, 100);
    limit(pda_hold_cmd, 0, 1024);
}

void Config::setOffset(int offset)
{
    this->offset = offset;
}
