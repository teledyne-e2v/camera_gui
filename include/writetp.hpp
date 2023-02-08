#pragma once
#include <stdio.h>
//#include <unistd.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/ioctl.h>
#include "i2c.hpp"

#define SENSOR_TIMING_PIXEL_FILENAME "../[2018-10-31]Tpix_Snappy2M_VOD.bin"
#define SENSOR_TIMING_PIXEL_DATA_SIZE 304
#define SENSOR_TIMING_PIXEL_HEADER_SIZE 128
#define WORD_SIZE 2

/**
 * @brief write Timing pixel
 * 
 */
void writeTP();

/**
 * @brief load Timing pixel
 * 
 * @param pixelData 
 * @return int 
 */
int loadTimingPixel(__u16 pixelData[]);
