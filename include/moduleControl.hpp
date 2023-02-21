#pragma once

#include "imgui.h"
#include "ModuleControl_v100.hpp"
#include "api.hpp"
#include <sys/time.h>
#include <vector>




/**
 * @brief Interface between application and module control
 * 
 */
struct Ext_Control{
    Control *control;
    int value;
    int previous_value;
    bool value_bool;
    bool previous_value_bool;
};

class ModuleControl
{
private:
    float expositionTimeVariation;
    int PDAVariation;
    int analogicGainVariation;
    int numericGainVariation;
    ModuleCtrl *moduleCtrl;

    /**
     * @brief exposition time 
     * 
     */
    float expositionTime = 15;

    /**
     * @brief PDA value
     * 
     */
    int PDA = 300;

    /**
     * @brief analogic gain
     * 
     */
    int analogicGain = 2;

    /**
     * @brief numerical gain
     * 
     */
    int numericGain = 0;

    /**
     * @brief Register to write
     * 
     */
    char registerToWrite[8] = "0x0000";

    /**
     * @brief value to write in the register
     * 
     */
    char valueToWrite[8] = "0x0000";

    /**
     * @brief register to read
     * 
     */
    char registerToRead[8] = "0x0000";

    /**
     * @brief register to read
     * 
     */
    char strRegisterValue[8] = "0x0000";

    /**
     * @brief value readed in the register
     * 
     */
    int readedValue = 0;

    float elementOffset = 180;

    /**
     * @brief 
     * Create the PDA Input int
     * 
     */
    void PDAConf();

    /**
     * @brief 
     *  Create the exposition time input int
     * 
     */
    void expositionTimeConf();

    /**
     * @brief 
     * Create the analogic gain input int
     * 
     */
    void analogicGainConf();

    /**
     * @brief 
     * Create the numeric gain input int
     * 
     */
    void numericGainConf();

    /**
     * @brief 
     * Create the write register input text
     * 
     * @param MDC 
     */
    void writeRegister();

    /**
     * @brief 
     * Create the read register input text
     * @param MDC 
     */
    void readRegister();

    void formatHex(char *value);
    void apply();
    void auto_controls();
    void auto_controls_render();
    std::vector<Ext_Control *> controls;

public:
    /**
     * @brief Render the window
     * 
     * @param MDC 
     */
    void render();

    /**
     * @brief define the display of the window
     * 
     */
    bool showWindow = false;

    void setPDA(int PDA);

    ModuleControl(ModuleCtrl *moduleCtrl);
};
