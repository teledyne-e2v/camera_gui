#include "moduleControl.hpp"

ModuleControl::ModuleControl(ModuleCtrl *moduleCtrl)
    : moduleCtrl(moduleCtrl)
{

#ifndef DEBUG_MODE
    auto_controls();
    int PdaRegValue;
    double PdaVoltageValue;
    moduleCtrl->read_VdacPda(&PdaRegValue, &PdaVoltageValue);

    this->setPDA(PdaRegValue);
#endif
}

void ModuleControl::apply()
{
    if (PDAVariation != PDA)
        moduleCtrl->write_VdacPda(PDA);

    for (auto &ctrl : controls)
    {
        if (ctrl->previous_value != ctrl->value)
        {
            set_control_by_code(ctrl->control->id, ctrl->value);
            ctrl->previous_value = ctrl->value;
        }
        else if (ctrl->previous_value_bool != ctrl->value_bool)
        {
            set_control_by_code(ctrl->control->id, ctrl->value_bool);
            ctrl->previous_value_bool = ctrl->value_bool;
        }
    }
}

void ModuleControl::render()
{
    if (showWindow)
    {
        ImGui::SetNextWindowSize(ImVec2(600, 250), ImGuiCond_FirstUseEver);
        ImGui::Begin("ModuleControl");

        ImGui::PushItemWidth(-1);
        PDAConf();
        auto_controls_render();
        readRegister();
        writeRegister();

        ImGui::PopItemWidth();
        ImGui::NewLine();
#ifndef DEBUG_MODE
        apply();
#endif

        ImGui::End();

        PDAVariation = PDA;
    }
}

void ModuleControl::auto_controls()
{
    Control_List *control_list;
    control_list = get_control_list();

    for (int i = 0; i < control_list->number_of_controls; i++)
    {

        if (strncmp(control_list->controls[i].name, "sensor_", 7) != 0)
        {
            Ext_Control *ext_ctrl = new Ext_Control();
            ext_ctrl->control = &(control_list->controls[i]);
            ext_ctrl->value = control_list->controls[i].default_value;
            ext_ctrl->previous_value = control_list->controls[i].default_value;
            ext_ctrl->value_bool = control_list->controls[i].default_value;
            ext_ctrl->previous_value_bool = control_list->controls[i].default_value;
            controls.push_back(ext_ctrl);
        }
    }
}

void ModuleControl::update_auto_controls()
{
    for (auto &ctrl : controls)
    {
        if (strcmp(ctrl->control->type, "bool") == 0)
        {
            ctrl->value_bool = get_control_by_code(ctrl->control->id);
            ctrl->previous_value_bool = ctrl->value_bool;
        }
        else
        {
            ctrl->value = get_control_by_code(ctrl->control->id);
            ctrl->previous_value = ctrl->value;
        }
    }
}

void ModuleControl::auto_controls_render()
{

    for (auto &ctrl : controls)
    {

        if (strcmp(ctrl->control->type, "bool") == 0)
        {
            ImGui::Text("%s",ctrl->control->name);
            ImGui::SameLine(elementOffset);
            if (ImGui::Checkbox(ctrl->control->name, &ctrl->value_bool))
                ;
        }
        else
        {
            ImGui::Text("%s",ctrl->control->name);
            ImGui::SameLine(elementOffset);
            ImGui::InputInt(ctrl->control->name, &ctrl->value, 1, 100, ImGuiInputTextFlags_CharsDecimal);
            if (ctrl->value < ctrl->control->minimum)
                ctrl->value = ctrl->control->minimum;
            else if (ctrl->value > ctrl->control->maximum)
                ctrl->value = ctrl->control->maximum;
        }
    }
}

void ModuleControl::PDAConf()
{
    ImGui::Text("PDA (-91 to 750):");
    ImGui::SameLine(elementOffset);
    ImGui::InputInt("##pda", &PDA, 1, 100, ImGuiInputTextFlags_CharsDecimal);
    if (PDA > 750)
        PDA = 750;
    else if (PDA < -91)
        PDA = -91;
}

void ModuleControl::readRegister()
{
    ImGui::NewLine();
    ImGui::Text("Read Register:");
    ImGui::Spacing();
    ImGui::Text("Register (Hex):");
    ImGui::SameLine(elementOffset);
    ImGui::InputText("##rreg", registerToRead, 7, ImGuiInputTextFlags_CharsHexadecimal);
    formatHex(registerToRead);
    ImGui::Text("Value (Hex):");
    ImGui::SameLine(elementOffset);
    sprintf(strRegisterValue, "0x%.4x", readedValue);
    ImGui::InputText("##regvalread", strRegisterValue, 7, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);

    ImGui::Spacing();

    if (ImGui::Button("Read"))
    {
        int registre;
        sscanf(registerToRead, "%x", &registre);
#ifndef DEBUG_MODE
        moduleCtrl->readReg(registre, &readedValue);
#endif
    }
}
void ModuleControl::writeRegister()
{
    ImGui::NewLine();
    ImGui::Text("Write into register:");
    ImGui::Spacing();
    ImGui::Text("Register (Hex):");
    ImGui::SameLine(elementOffset);
    ImGui::InputText("##wreg", registerToWrite, 7, ImGuiInputTextFlags_CharsHexadecimal);

    ImGui::Text("Value (Hex):");
    ImGui::SameLine(elementOffset);
    ImGui::InputText("##regval", valueToWrite, 7, ImGuiInputTextFlags_CharsHexadecimal);
    formatHex(registerToWrite);
    formatHex(valueToWrite);

    ImGui::Spacing();

    if (ImGui::Button("Write"))
    {
        int registre, value;
        sscanf(registerToWrite, "%x", &registre);
        sscanf(valueToWrite, "%x", &value);
#ifndef DEBUG_MODE
        moduleCtrl->writeReg(registre, value);
        apply();
#endif
    }
}

void ModuleControl::setPDA(int PDA)
{
    this->PDA = PDA;
}

void ModuleControl::formatHex(char *value)
{
    size_t len = strlen(value);

    if (value[1] != 'x')
    {
        if (len > 4)
            len = 4;

        // Move least signigicants bits to the left of the string
        for (size_t k = 0; k < 6 - len; k++)
        {
            for (int i = 5; i >= 1; i--)
            {
                value[i] = value[i - 1];
            }
        }

        // Fill most significant bits to zeros
        for (size_t i = 2; i < 2 + (4 - len); i++)
        {
            value[i] = '0';
        }

        value[0] = '0';
        value[1] = 'x';
        value[7] = '\0';
    }
}
