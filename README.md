# Version 2.4

# About

This application aims to simplify the use of autofocus and sensor control.

# Dependencies

	- libglfw3-dev
	- libglfw3
	- meson
	- ninja
    - autofocus plugin
    - barcodereader plugin
    - freeze plugin

# Auto-installation script


To auto-install all dependencies (except the plugins) and auto-compile, run :
	./install.sh

To install the plugins refer to their documentation


# Manual Compilation (useless if script worked)
 
First you must make sure that your device's clock is correctly setup.
Otherwise the compilation will fail.

Move to the ImGuiInterface directory using the command cd

Then do:
    meson build

And finally:
    ninja -C build

Note: if some dependencies are missing, meson will signal it.

# Usage

run:

- start.sh

You should see the interface with the video stream.

if it doesn't work :

- cd build
- export GST_PLUGIN_PATH=/usr/local/lib/gstreamer-1.0/
- export LD_LIBRARY_PATH=/usr/local/lib/
- ./gst-imgui

# Explanation

## Autofocus Controle

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
