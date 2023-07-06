# Version 3.0

# About

This application aims to simplify the use of autofocus and sensor control.

It is running on **Nvidia Jetson Nano** and require **OPTIMOM 2M** driver installed on the system.

# Dependencies
Library dependencies are:
- v4l-utils
- libv4l-dev
- libgstreamer1.0-dev
- libgstreamer-plugins-base1.0-dev
- libglfw3-dev
- libglfw3



Install them with: 

	sudo apt install v4l-utils libv4l-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libglfw3-dev libglfw3 meson
	
	
## Topaz api install
Take a look at this github: 
https://github.com/teledyne-e2v/topaz-api

## Plugins install

The application need the following gstreamer plugin to run properly (it can run without but will be less interesting):
- [gst-autofocus](https://github.com/teledyne-e2v/gst-autofocus) (required version : 1.3)
- [gst-multifocus](https://github.com/teledyne-e2v/gst-multifocus) (required version : 2.0)
- [gst-autoexposure](https://github.com/teledyne-e2v/gst-autoexposure) (required version : 1.0)
- [gst-barcode-reader](https://github.com/teledyne-e2v/gst-barcode-reader) (required version : 1.3)
- [gst-freeze](https://github.com/teledyne-e2v/gst-freeze) (required version : 1.0)
- [gst-sharpness](https://github.com/teledyne-e2v/gst-sharpness) (required version : 1.0)

Color plugins :
- [gst-fpscounter](required version 1.0)
- bayer2rgb (can be installed with ```sudo apt install gstreamer1.0-plugins-bad```)
- [gst-gray2bayer](https://github.com/teledyne-e2v/gst-gray2bayer) (required version 1.0)
- [gst-whitebalance](https://github.com/teledyne-e2v/gst-whitebalance) (required version : 1.0)

Plese refer to their git for a complete installation procedure

# Compilation
First you must make sure that your device's clock is correctly setup on the Jetson Nano.
Otherwise the compilation will fail.

Move to the **camera_gui** directory using the command cd

Then do:

    meson build

And finally:

    ninja -C build

Note: if some dependencies are missing, meson will signal it.

# Usage

Execute the following srcipt to start the application:

	bash start.sh

You should see the interface with the video stream.

If the start script doesn't work, you can execute manually:

	cd build
	export GST_PLUGIN_PATH=/usr/local/lib/gstreamer-1.0/
	export LD_LIBRARY_PATH=/usr/local/lib/
	./gst-imgui

# Camera GUI manual

## Autofocus Control

Allows the user to do the autofocus and change the ROI

### ROI Centered Button

ROI centered button opens a window to define the size of an ROI centered on the screen.

### Autofocus Button

The autofocus button is used to start the autofocus (using the method chosen in the settings).

### Change ROI radio button

The change ROI radio button allows you to define the ROI by selecting an area with the mouse.

### Move ROI radio button 

The move ROI button allows you to move the ROI with drag and drop.

### Calibrating

Calcul the response time and set his value into the autofocus algorithm.

## Debug window 

Display the debug of the autofocus algorithm with all steps with sharpness calculations

## Take photo

Panel which can be used to save photos displayed in the graphique interface.

## Module control panel

The module control panel implements the functions present in ModuleControl, please refer to the Module control documentation for more informations.

## Configuration panel

The configuration panel allows you to configure the autofocus, please refer to the autofocus plugin documentation for more informations.

## Barcode config

The configuration panel allows you to configure the barcodereader, please refer to the barcodereader plugin documentation for more informations.

## White balance 

The configuration panel allows you to configure the whitebalance, please refer to the whitebalance plugin documentation for more informations.

## Histograms 

This window can enable the display and calcultation of mono and color histograms 
