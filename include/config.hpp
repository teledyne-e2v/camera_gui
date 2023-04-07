#pragma once
#include <gst/gst.h>

#include "utils.hpp"

/**
 * @brief configuration of the autofocus plugin and show the menu.
 *
 */
class Config
{
private:
    /**
     * @brief
     * Define the strategie used to autofocus
     * Default value is defined by the plugin
     */
    int strategy = 1;
    int previousStrategy = 1;
    /**
     * @brief
     * Define the small step of the autofocus algorithm.
     * More information in the plugin documentation.
     * Default value is defined by the plugin.
     */
    int smallStep = 8;
    int previousSmallStep = 8;
    /**
     * @brief
     * Define the big step of the autofocus algorithm.
     * More information in the plugin documentation.
     * Default value is defined by the plugin.
     */
    int bigStep = 63;
    int previousBigStep = 63;
    /**
     * @brief
     * Define the minPDA of the autofocus algorithm.
     * More information in the plugin documentation.
     * Default value is defined by the plugin.
     */
    int pdaMin = -90;
    int previousPdaMin = -90;
    /**
     * @brief
     * Define the maxPDA of the autofocus algorithm.
     * More information in the plugin documentation.
     * Default value is defined by the plugin.
     */
    int pdaMax = 750;
    int previousPdaMax = 750;
    /**
     * @brief
     * Define the dec of the autofocus algorithm.
     * More information in the plugin documentation.
     * Default value is defined by the plugin.
     */
    int dec = 2;
    int previousDec = 2;

    /**
     * @brief
     * Define the offset of the autofocus algorithm.
     * More information in the plugin documentation.
     * Default value is defined by the plugin.
     */
    int offset = 3;
    int previousOffset = 3;

    /**
     * @brief
     * Define the continuous autofocus in bool value.
     *
     * More information in the plugin documentation.
     * Default value is defined by the plugin.
     */
    bool cont = false;
    bool previousCont = false;
    /**
     * @brief
     * Define the continuous_update_interval of the autofocus plugin.
     *
     * More information in the plugin documentation.
     * Default value is defined by the plugin.
     */
    int continuous_update_interval = 30;
    int previous_continuous_update_interval = 30;
    /**
     * @brief
     * Define the continuous_threshold of the autofocus plugin.
     *
     * More information in the plugin documentation.
     * Default value is defined by the plugin.
     */
    float continuous_threshold = 30;
    float previous_contiuous_threshold = 30;
    /**
     * @brief
     * Define the continuous_timeout of the autofocus plugin.
     *
     * More information in the plugin documentation.
     * Default value is defined by the plugin.
     */
    int continuous_timeout = 5;
    int previous_continuous_timeout = 5;
    /**
     * @brief Autfocus gst-plugin
     *
     */
    GstElement *autofocus;

    /**
     * @brief put all values into camera register.
     *
     * Calls for ModuleControlv100 and i2c
     */
    void apply();

    int pda_hold_cmd = 0;

    int previous_pda_hold_cmd = 0;

    static const int nbAlgos = 2;

public:
    /**
     * @brief Define the display of window, if true, display window.
     *
     */
    bool showWindow = false;

    /**
     * @brief render the window
     *
     */
    void render();
    /**
     * @brief
     * Check the values of the autofocus parameters following the autofocus plugin documentation.
     *
     */
    void security();
    /**
     * @brief Construct a new Config object
     *
     * @param autofocus
     */
    Config(GstElement *autofocus);
    void setOffset(int offset);
};
