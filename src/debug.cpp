#include "debug.hpp"
#include "utils.hpp"
// #define DEBUG_MODE

Debug::Debug(GstElement *autofocus)
{
    this->autofocus = autofocus;
    strncpy(this->logs, "empty", 6);
}

void Debug::render(bool autofocusTrigger)
{
    ImGui::Begin("Debug");
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly;

    ImGui::PushItemWidth(-1);
    ImGui::InputTextMultiline("##", logs, strlen(logs), ImGui::GetWindowSize() - ImVec2(15, 75), flags);
    ImGui::PopItemWidth();

    ImGui::NewLine();

    if (ImGui::Button("Copy"))
    {
        ImGui::SetClipboardText(logs);
    }

    if (autofocusTrigger)
        updateLogs();

    ImGui::End();
}

void Debug::updateLogs()
{
#ifndef DEBUG_MODE
    char *message;
    g_object_get(G_OBJECT(autofocus), "debug", &message, NULL);
    if (message != NULL)
    {
        strncpy(logs, message, 20000);
        free(message);
        logs[20000 - 1] = 0;
    }
#endif
}
