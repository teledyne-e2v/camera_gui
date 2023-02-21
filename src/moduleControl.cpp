#include "moduleControl.hpp"

ModuleControl::ModuleControl(ModuleCtrl *moduleCtrl)
    : moduleCtrl(moduleCtrl)
{

#ifndef DEBUG_MODE
    int PdaRegValue;
    double PdaVoltageValue;
    moduleCtrl->read_VdacPda(&PdaRegValue, &PdaVoltageValue);

    this->setPDA(PdaRegValue);
#endif
}

void ModuleControl::apply()
{
    if (expositionTime != expositionTimeVariation)
            moduleCtrl->setTint(expositionTime);
        if (analogicGainVariation != analogicGain)
            moduleCtrl->setAnalogGain(analogicGain);
        if (numericGainVariation != numericGain)
            moduleCtrl->setDigitalGain(numericGain);
        if (PDAVariation != PDA)
            moduleCtrl->write_VdacPda(PDA);
}

void ModuleControl::render()
{
    if (showWindow)
    {
        ImGui::SetNextWindowSize(ImVec2(600, 250), ImGuiCond_FirstUseEver);
        ImGui::Begin("ModuleControl");

        ImGui::PushItemWidth(-1);
        PDAConf();

        expositionTimeConf();
        analogicGainConf();

        numericGainConf();

        readRegister();
        writeRegister();
	auto_controls();
        ImGui::PopItemWidth();
        ImGui::NewLine();
#ifndef DEBUG_MODE
        apply();
#endif
        ImGui::End();

        expositionTimeVariation = expositionTime;
        analogicGainVariation = analogicGain;
        numericGainVariation = numericGain;
        PDAVariation = PDA;
    }
}

void ModuleControl::auto_controls()
{
    Control_List *control_list ;
    control_list=get_control_list();
	
    for(int i=0;i<control_list->number_of_controls;i++)
    {
	printf("%s\n",control_list->controls[i].name);
        if(strcmp(control_list->controls[i].type,"bool")==0)
        {
            ImGui::Text(control_list->controls[i].name);
            ImGui::SameLine(elementOffset);
            bool tmp;
            if (ImGui::Checkbox("##tmp", &tmp))
            {

            }
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

void ModuleControl::expositionTimeConf()
{
    ImGui::Text("Exposition time:");
    ImGui::SameLine(elementOffset);
    ImGui::InputFloat("##expTime", &expositionTime, 1, 100, "%.2f ms");
    if (expositionTime <= 0)
        expositionTime = 1;
    else if (expositionTime > 500)
        expositionTime = 500;
}

void ModuleControl::analogicGainConf()
{
    ImGui::Text("Analogic gain (1 to 8):");
    ImGui::SameLine(elementOffset);
    ImGui::InputInt("##Again", &analogicGain, 1, 100, ImGuiInputTextFlags_CharsDecimal);
    if (analogicGain <= 0)
        analogicGain = 1;
    else if (analogicGain > 8)
        analogicGain = 8;
}

void ModuleControl::numericGainConf()
{
    ImGui::Text("Numeric gain:");
    ImGui::SameLine(elementOffset);
    ImGui::InputInt("##NGain", &numericGain, 1, 100, ImGuiInputTextFlags_CharsDecimal);
    if (numericGain <= 0)
        numericGain = 1;
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
