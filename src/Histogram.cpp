#include "Histogram.hpp"
#include "external/implot/implot.h"
#include "utils.hpp"
#include <math.h>
#include <numeric>
#include <stdio.h>
#include <string>
Histogram::Histogram(ROI *Roi) : Roi(Roi) {
  for (int i = 0; i < DEPTH; i++) {
    X[i] = i;
  }
}

void Histogram::calculate_histogram_gray(guint8 *image) {
  ImVec4 roi = Roi->getROI();
  for (int i = 0; i < DEPTH; i++) {
    histogram[i] = 0;
  }

  for (int j = roi.x; j < roi.z - 1; j += 1) {
    for (int i = roi.y; i < roi.w - 1; i += 1) {
      histogram[image[i * 1920 + j]] += 1;
    }
  }
}

void Histogram::calculate_histogram_gray_RGBA(guint8 *image) {
  ImVec4 roi = Roi->getROI();
  roi.x = roi.x - ((int)roi.x % 2);
  roi.y = roi.y - ((int)roi.y % 2);
  roi.z = roi.z - ((int)roi.z % 2);
  roi.w = roi.w - ((int)roi.w % 2);
  for (int i = 0; i < DEPTH; i++) {
    histogram[i] = 0;
  }

  for (int j = roi.x; j < roi.z - 2; j += 1) {
    for (int i = roi.y; i < roi.w - 2; i += 1) {

      histogram[image[i * 4 * 1920 + j * 4 + 0]]++;
      histogram[image[i * 4 * 1920 + j * 4 + 1]]++;
      histogram[image[i * 4 * 1920 + j * 4 + 2]]++;
    }
  }
}
void Histogram::calculate_histogram_color(guint8 *image) {
  ImVec4 roi = Roi->getROI();
  roi.x = roi.x - ((int)roi.x % 2);
  roi.y = roi.y - ((int)roi.y % 2);
  roi.z = roi.z - ((int)roi.z % 2);
  roi.w = roi.w - ((int)roi.w % 2);
  for (int i = 0; i < DEPTH; i++) {
    histogram_red[i] = 0;
    histogram_green[i] = 0;
    histogram_blue[i] = 0;
  }

  for (int j = roi.x; j < roi.z - 2; j += 2) {
    for (int i = roi.y; i < roi.w - 2; i += 2) {

      histogram_red[image[i * 1920 + j]] += 1;
      histogram_green[image[i * 1920 + j + 1]] += 1;
      histogram_green[image[(i + 1) * 1920 + j]] += 1;
      histogram_blue[image[(i + 1) * 1920 + j + 1]] += 1;
    }
  }
}

void Histogram::calculate_histogram_color_RGBA(guint8 *image) {
  ImVec4 roi = Roi->getROI();
  roi.x = roi.x - ((int)roi.x % 2);
  roi.y = roi.y - ((int)roi.y % 2);
  roi.z = roi.z - ((int)roi.z % 2);
  roi.w = roi.w - ((int)roi.w % 2);
  for (int i = 0; i < DEPTH; i++) {
    histogram_red[i] = 0;
    histogram_green[i] = 0;
    histogram_blue[i] = 0;
  }

  for (int j = roi.x; j < roi.z - 2; j += 1) {
    for (int i = roi.y; i < roi.w - 2; i += 1) {

      histogram_red[image[i * 4 * 1920 + j * 4 + 0]]++;
      histogram_green[image[i * 4 * 1920 + j * 4 + 1]]++;
      histogram_blue[image[i * 4 * 1920 + j * 4 + 2]]++;
    }
  }
}

int histogram_center(int *histogram, int size) {
  int sum = std::accumulate(histogram, histogram + size, 0);
  int tmp_sum = 0;
  for (int i = 0; i < size; i++) {
    tmp_sum += histogram[i];
    if (tmp_sum >= sum / 2) {
      return i;
    }
  }
  return size - 1;
}

float histogram_std(int *histogram, int size, int mid_point) {
  long int sum = 0;
  for (int i = 0; i < size; i++) {
    sum += histogram[i] * pow((i - mid_point), 2);
  }
  sum = sum / (std::accumulate(histogram, histogram + size, 0));
  return sqrt(sum);
}

void histogram_infos(int *histogram, int size, int *min, int *max,
                    int *low_satured, int *high_satured) {
  for (int i = 0; i < size; i++) {
    if (histogram[i] > 0) {
      *min = i;
      break;
    }
  }

  for (int i = size - 1; i > 0; i--) {
    if (histogram[i] > 0) {
      *max = i;
      break;
    }
  }
  *low_satured = histogram[0];
  *high_satured = histogram[size - 1];
}

void Histogram::render(guint8 *image, bool RGBA, bool frame_created) {

  ImGui::Begin("Histograms");

  if (ImGui::Button("Mono histogram calcul")) {
    waiting_histogram = true;
  }
  if (waiting_histogram && frame_created) {
    if (RGBA) {
      calculate_histogram_gray_RGBA(image);
    } else {
      calculate_histogram_gray(image);
    }
    waiting_histogram = false;
  }
  ImGui::SameLine();
  ImGui::Checkbox("##Toggle histogram", &toggle_histogram);

  ImGui::Text("Display histogram");
  ImGui::SameLine();
  ImGui::Checkbox("##Display histogram", &display_histogram);

  if (ImGui::Button("Color histogram calcul")) {
    waiting_histogram_color = true;
  }
  if (waiting_histogram_color && frame_created) {
    if (RGBA) {
      calculate_histogram_color_RGBA(image);
    } else {
      calculate_histogram_color(image);
    }
    waiting_histogram_color = false;
  }
  ImGui::SameLine();
  ImGui::Checkbox("##Toggle histogram color", &toggle_histogram_color);

  ImGui::Text("Display histogram color");
  ImGui::SameLine();
  ImGui::Checkbox("##Display histogram color", &display_histogram_color);

  if (toggle_histogram_color && frame_created) {
    if (RGBA) {
      calculate_histogram_color_RGBA(image);
    } else {
      calculate_histogram_color(image);
    }
  }
  if (toggle_histogram && frame_created) {
    if (RGBA) {
      calculate_histogram_gray_RGBA(image);
    } else {
      calculate_histogram_gray(image);
    }
  }

  if (display_histogram) {
    if (ImPlot::BeginPlot("Mono Histogram",ImVec2(-1,0),ImPlotAxisFlags_AutoFit)) {
      ImPlot::PlotLine("Histogram", X, histogram, DEPTH);
      ImPlot::EndPlot();
    }

    int midpoint = histogram_center(histogram, DEPTH);
    ImGui::Text("Average : %d", midpoint);
    ImGui::Text("Standard deviation : %f",
                histogram_std(histogram, DEPTH, midpoint));

    int min, max, low_satured, high_satured;
    histogram_infos(histogram, DEPTH, &min, &max, &low_satured, &high_satured);

    ImGui::Text("Min : %d", min);
    ImGui::Text("Max : %d", max);
    ImGui::Text("Totally dark pixels : %d", low_satured);
    ImGui::Text("Saturated pixels : %d", high_satured);
  }

  if (display_histogram_color) {
    if (ImPlot::BeginPlot("COLOR histogram",ImVec2(-1,0),ImPlotAxisFlags_AutoFit)) {

      ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.0, 0.5, 0.5, 1.0));
      ImPlot::PlotLine("RED histogram", X, histogram_red, DEPTH);
      ImPlot::PopStyleColor();

      ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.5, 1.0, 0.5, 1.0));
      ImPlot::PlotLine("GREEN histogram", X, histogram_green, DEPTH);
      ImPlot::PopStyleColor();

      ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.5, 0.5, 1.0, 1.0));
      ImPlot::PlotLine("BLUE histogram", X, histogram_blue, DEPTH);
      ImPlot::PopStyleColor();

      ImPlot::EndPlot();

      ImGui::BeginTable("Table", 4);
      ImGui::TableNextColumn();
      ImGui::Text("COLOR");


      ImGui::TableNextColumn();
      ImGui::Text("RED");
      ImGui::TableNextColumn();
      ImGui::Text("GREEN");
      ImGui::TableNextColumn();
      ImGui::Text("BLUE");
      
      ImGui::TableNextRow();

      int midpoint[3];
      int min[3], max[3], low_satured[3], high_satured[3];
      float std[3];
      //std::string names[6]= {"Average","Standard deviation","Min","Max","Zero's pixels","Saturated pixels "} 
      for (int i = 0; i < 3; i++) { if (i == 0) used_hist = histogram_red; else if (i == 1)
          used_hist = histogram_green;
        else
          used_hist = histogram_blue;

        midpoint[i] = histogram_center(used_hist, DEPTH);
        std[i] = histogram_std(used_hist, DEPTH, midpoint[i]);
        histogram_infos(used_hist, DEPTH, min + i, max + i, low_satured + i,
                        high_satured + i);
      }
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Average");
      
      for(int col =1;col<4; col++)
      {
        ImGui::TableSetColumnIndex(col);
        ImGui::Text("%d",midpoint[col-1]);
      }

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Std");

      for(int col =1;col<4; col++)
      {
        ImGui::TableSetColumnIndex(col);
        ImGui::Text("%.2f",std[col-1]);
      }


      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Min");

      for(int col =1;col<4; col++)
      {
        ImGui::TableSetColumnIndex(col);
        ImGui::Text("%d",min[col-1]);
      }
            ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Max");

      for(int col =1;col<4; col++)
      {
        ImGui::TableSetColumnIndex(col);
        ImGui::Text("%d",max[col-1]);
      }
            ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Zero's pixels");

      for(int col =1;col<4; col++)
      {
        ImGui::TableSetColumnIndex(col);
        ImGui::Text("%d",low_satured[col-1]);
      }
            ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Saturated pixels");

      for(int col =1;col<4; col++)
      {
        ImGui::TableSetColumnIndex(col);
        ImGui::Text("%d",high_satured[col-1]);
      }
      
      ImGui::EndTable();
    }
  }

  ImGui::End();
}
