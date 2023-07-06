# v3.0

## Color support

- Support the color sensor using v4l2 gray2bayer plugin + bayer2rgb plugin
- Application can display RGBA images 

### Auto white balance  (version >= 1.0)

- Support all parameters of the plugin

### Debayer

- Configuration of the debayerisation

## Sensor informations 

- Read the EEProm at the launch to display some usefull informations about the sensor

## Histograms

- Application can display mono histograms and color histograms
- Can be calculated 2 differents way (using the RGBA image or the gray8 image)
- Display some statistics from the histogram

## Sharpness Plugin 

- Add the sharpness plugin to debug the autofocus
- Allos us to draw curves with sharpness in function of the DAC


## Autofocus 

- New autofocus algorithms support
- Can enable/disable the continuous sharpness calculation
- Changed some default values

## Multifocus (version >= 2.0): 

- Update using new plugin
- Number of plans parameters support

## Framerate informations 

- Display the framerate of the differents elements (Gstreamer, sensor, application)


# v2.4.1 

- FIX take photo crashes
