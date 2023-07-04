#include "Autofocus.hpp"
#include "utils.hpp"
#include <sstream>
#include <stdio.h>

typedef enum { PENDING, IN_PROGRESS, WAITING, COMPLETED } AutofocusStatus;

AutofocusControl::AutofocusControl(GstElement *autofocus,
                                   ModuleCtrl *moduleCtrl,
                                   ModuleControl *moduleControl, Config *conf,
                                   ROI *Roi)
    : autofocus(autofocus), moduleCtrl(moduleCtrl),
      moduleControl(moduleControl), conf(conf), Roi(Roi) {}

AutofocusControl::~AutofocusControl() {}

void AutofocusControl::updatePluginROI() {
#ifndef DEBUG_MODE
  ImVec4 roi = Roi->getROI();

  if (roi.w != previousRoi.w || roi.x != previousRoi.x ||
      roi.y != previousRoi.y || roi.z != previousRoi.z) {

    g_object_set(G_OBJECT(autofocus), "x", (int)roi.x, "y", (int)roi.y, "width",
                 (int)(roi.z - roi.x), "height", (int)(roi.w - roi.y), NULL);
  }

#endif
}

bool AutofocusControl::render(ImDrawList *drawList, ImVec2 streamSize,
                              ImVec2 streamPosition, ImVec2 windowSize,
                              ImVec2 windowPosition) {
  ImGui::Begin("Autofocus Control");

  // Button to launch autofocus
  if (ImGui::Button("Autofocus", buttonSize) && streamSize.y != 0 &&
      streamSize.x != 0) {
#ifndef DEBUG_MODE
    updatePluginROI();
    g_object_set(G_OBJECT(autofocus), "autofocusStatus", PENDING, NULL);

    startingAutofocus = true;
#endif
  }
  if (!startingAutofocus && isAutofocusDone)
    isAutofocusDone = false;
  if (startingAutofocus) {
#ifndef DEBUG_MODE
    AutofocusStatus autofocusState;
    g_object_get(G_OBJECT(autofocus), "autofocusStatus", &autofocusState, NULL);

    if (autofocusState == COMPLETED) {
      isAutofocusDone = true;
      int PdaRegValue;
      double PdaVoltageValue;
      moduleCtrl->read_VdacPda(&PdaRegValue, &PdaVoltageValue);
      moduleControl->setPDA(PdaRegValue);
      startingAutofocus = false;
    }
#endif
  }

  if (ImGui::Button("Calibrating", buttonSize)) {
    updatePluginROI();
    g_object_set(G_OBJECT(autofocus), "calibrating", true, NULL);
    calibrating = true;
  }

  ImGui::Spacing();

  bool focus_lost = 0;

  g_object_get(G_OBJECT(autofocus), "focus_lost", &focus_lost, NULL);

  if (focus_lost)
    updatePluginROI();

  ImGui::Checkbox("Display lost focus", &displayRedRectangle);
  ImGui::Checkbox("Sharpness calculation", &sharpnessCalculation);
  if(previouSharpnessCalculation != sharpnessCalculation)
  {
    g_object_set(G_OBJECT(autofocus), "sharpnessCalculation", sharpnessCalculation, NULL);
    previouSharpnessCalculation = sharpnessCalculation;
  }
  if (calibrating) {
    g_object_get(G_OBJECT(autofocus), "calibrating", &calibrationState, NULL);

    if (!calibrationState) {
      int offset = 0;
      g_object_get(G_OBJECT(autofocus), "offset", &offset, NULL);
      int PdaRegValue;
      double PdaVoltageValue;
      moduleCtrl->read_VdacPda(&PdaRegValue, &PdaVoltageValue);
      moduleControl->setPDA(PdaRegValue);
      conf->setOffset(offset);
      calibrating = false;
    }
  }

  long int sharpness = 0;
  int pda = 0;

#ifndef DEBUG_MODE
  double PdaVoltageValue = 0;

  g_object_get(G_OBJECT(autofocus), "sharpness", &sharpness, NULL);
  moduleCtrl->read_VdacPda(&pda, &PdaVoltageValue);
#endif

  ImGui::NewLine();
  ImGui::Text("Sharpness: %ld", sharpness);
  ImGui::Spacing();
  ImGui::Text("PDA: %d", pda);

  ImGui::End();
  return displayRedRectangle && focus_lost;
}
