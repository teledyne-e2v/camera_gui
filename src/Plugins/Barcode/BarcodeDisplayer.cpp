#include "BarcodeDisplayer.hpp"

BarcodeDisplayer::BarcodeDisplayer(GstElement *barcodereader)
    : barcodereader(barcodereader)
{
    strncpy(this->barcodes, "empty", 6);
}

void BarcodeDisplayer::render()
{
    ImGui::Begin("Decoded barcodes");
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly;

    ImGui::PushItemWidth(-1);
    ImGui::InputTextMultiline("##", barcodes, strlen(barcodes), ImGui::GetWindowSize() - ImVec2(15, 75), flags);
    ImGui::PopItemWidth();

    ImGui::NewLine();

    if (ImGui::Button("Copy"))
    {
        ImGui::SetClipboardText(barcodes);
    }

    ImGui::SameLine();

    if (ImGui::Button("Clear"))
    {
#ifndef DEBUG_MODE
        g_object_set(G_OBJECT(barcodereader), "clearBarcode", true, NULL);
#endif
    }
    updateLogs();
    ImGui::End();
}

void BarcodeDisplayer::updateLogs()
{

#ifndef DEBUG_MODE
    char *message;
    g_object_get(G_OBJECT(barcodereader), "strCode", &message, NULL);
    if (message != NULL)
    {
        if (strlen(message) == 0)
        {
            strncpy(this->barcodes, "empty", 6);
        }
        else
        {
            strncpy(barcodes, message, 80000);
            free(message);
            barcodes[80000 - 1] = 0;
        }
    }

#endif
}
