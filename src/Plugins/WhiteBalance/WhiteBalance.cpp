#include "WhiteBalance.hpp"
#include "external/implot/implot.h"
#include "utils.hpp"
#include <stdio.h>
WhiteBalanceControl::WhiteBalanceControl(GstElement *whitebalance, ROI *Roi)
    : whitebalance(whitebalance), Roi(Roi) {}

void WhiteBalanceControl::apply_ROI() {
  ImVec4 roi = Roi->getROI();
  printf("ROI : %f %f %f %f \n", roi.x, roi.y, roi.z, roi.w);
  g_object_set(G_OBJECT(whitebalance), "roi1x", (int)roi.x, "roi1y", (int)roi.y,
               "roi2x", (int)roi.z, "roi2y", (int)roi.w, NULL);
}

WhiteBalanceControl::~WhiteBalanceControl() {}

void WhiteBalanceControl::render() {
  bool white_balance_done;
  g_object_get(G_OBJECT(whitebalance), "autowhitebalance", &white_balance_done,
               NULL);
  if (auto_white_balance_toggled && !white_balance_done) {
    g_object_get(G_OBJECT(whitebalance), "red", &red, NULL);
    g_object_get(G_OBJECT(whitebalance), "blue", &blue, NULL);
    g_object_get(G_OBJECT(whitebalance), "green", &green, NULL);
    previous_red = red;
    previous_green = green;
    previous_blue = blue;
    auto_white_balance_toggled = false;
  }

  ImGui::Begin("White Balance");

  if (ImGui::Button("Auto white balance")) {

    apply_ROI();
    auto_white_balance_toggled = true;
    g_object_set(G_OBJECT(whitebalance), "autowhitebalance", true, NULL);
  }

  if (ImGui::Button("Reset")) {
    g_object_set(G_OBJECT(whitebalance), "red", 256, NULL);
    g_object_set(G_OBJECT(whitebalance), "green", 256, NULL);
    g_object_set(G_OBJECT(whitebalance), "blue", 256, NULL);
    red=256;
    green=256;
    blue=256;
  }

  ImGui::Text("RED");
  ImGui::SameLine();
  ImGui::InputInt("##RED sharpness", &red, 1, 10,
                  ImGuiInputTextFlags_CharsDecimal);
  if (red != previous_red) {
    g_object_set(G_OBJECT(whitebalance), "red", red, NULL);
    previous_red = red;
  }

  ImGui::Text("GREEN");
  ImGui::SameLine();
  ImGui::InputInt("##GREEN sharpness", &green, 1, 10,
                  ImGuiInputTextFlags_CharsDecimal);
  if (green != previous_green) {
    g_object_set(G_OBJECT(whitebalance), "green", green, NULL);
    previous_green = green;
  }

  ImGui::Text("BLUE");
  ImGui::SameLine();
  ImGui::InputInt("##BLUE sharpness", &blue, 1, 10,
                  ImGuiInputTextFlags_CharsDecimal);
  if (blue != previous_blue) {
    g_object_set(G_OBJECT(whitebalance), "blue", blue, NULL);
    previous_blue = blue;
  }

  ImGui::End();
}
