#pragma once

#include "imgui.h"
#include "roi.hpp"
#include <gst/app/app.h>
#include <gst/gst.h>
#include <gst/video/video.h>
#include <map>

#define DEPTH 256

class Histogram {
public:
  Histogram(ROI *Roi);
  ~Histogram();
  void render(guint8 *image, bool RGBA, bool frame_created);

private:
  bool waiting_histogram = false;
  bool waiting_histogram_color = false;

  bool toggle_histogram = false;
  bool toggle_histogram_color = false;
  bool display_histogram = false;
  bool display_histogram_color = false;
  ROI *Roi;
  void calculate_histogram_color(guint8 *image);
  void calculate_histogram_color_RGBA(guint8 *image);

  void calculate_histogram_gray(guint8 *image);
  void calculate_histogram_gray_RGBA(guint8 *image);

  int histogram[DEPTH];
  int histogram_red[DEPTH];
  int histogram_green[DEPTH];
  int histogram_blue[DEPTH];
  int X[DEPTH];
  int *used_hist = histogram_red;
};
