#pragma once
#include "i2c.hpp"
#include "writetp.hpp"

/**
 * @brief 
 * 
 * @param device 
 * @param bus 
 * @return int 
 */
int enable_VdacPda(I2CDevice device, int bus);

/**
 * @brief 
 * 
 * @param device 
 * @param bus 
 * @return int 
 */
int desable_VdacPda(I2CDevice device, int bus);


/**
 * @brief This class is used to communicate with the camera module
 * 
 */
class ModuleCtrl
{
private:
    /**
     * @brief 
     * Bus number
     * 
     */
    int bus;

    /**
     * @brief 
     * Name of the ic2 bus
     * 
     */
    char bus_name[32] = "/dev/i2c-6"; //<--bus 6

    /**
     * @brief 
     * PDA IC2 device (control lens)
     */
    I2CDevice devicepda;

    /**
     * @brief 
     * IC2 Device (control module)
     */
    I2CDevice device;

public:
    /**
     * @brief 
     * Initialise IC2 connection and load drivers
     */
    void ModuleControlInit();

    ~ModuleCtrl();

    /**
     * @brief 
     * Close IC2 connection
     * 
     */
    void ModuleControlClose();

    /**
     * @brief Read the state of the sensor
     * 
     * @return int The sensor state value
     */
    int read_sensor_state();

    /**
     * @brief Set the exposition time
     * 
     * @param b 
     * @return int 
     */
    int setTint(float b);

    /**
     * @brief Set the Analog Gain
     * 
     * @param b 
     * @return int 
     */
    int setAnalogGain(float b);

    /**
     * @brief Set the Digital Gain
     * 
     * @param b1 
     * @return int 
     */
    int setDigitalGain(float b1);

    /**
     * @brief 
     * Set the PDA
     * 
     * @param PdaRegValue 
     * @return int 
     */
    int write_VdacPda(int PdaRegValue);

    /**
     * @brief write specified value in the specified register
     * 
     * @param registre 
     * @param value 
     * @return int 
     */
    int writeReg(int registre, int value);

    /**
     * @brief 
     * Read specified register
     * 
     * @param registre 
     * @param value 
     * @return int 
     */
    int readReg(int registre, int *value);

    int read_VdacPda( int *PdaRegValue, double *PdaVoltageValue);

};

struct solution
{
    int tintII;
    int tintclk;
};
