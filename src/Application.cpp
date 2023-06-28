#include "Application.hpp"
#include <chrono>
#include <fstream>
#include <math.h>
#include <unistd.h>
Application::Application(int argc, char **argv) {
  int err = 0;

  loadImGuiConfig();
  moduleCtrl = new ModuleCtrl();

  err = moduleCtrl->ModuleControlInit(); // init ic2

  window = new Window();

  sensor = new Sensor(*moduleCtrl);

  pipeline = new Pipeline(argc, argv, sensor->getColor());

  moduleControlConfig = new ModuleControl(moduleCtrl);
  Roi = new ROI();
  freeze = pipeline->getImageFreeze();
  fpscounter = pipeline->getFpscounter();

  if (err == 0 && sensor->getMultifocus()) {
    autofocus = pipeline->getAutofocus();
    multifocus = pipeline->getMultifocus();
    sharpness = pipeline->getSharpness();
  }

  if (autofocus) {
    autofocusConfig = new Config(autofocus);
    autofocusControl = new AutofocusControl(
        autofocus, moduleCtrl, moduleControlConfig, autofocusConfig, Roi);
    autofocusDebug = new Debug(autofocus);
  }

  barcodereader = pipeline->getBarcodeReader();
  if (barcodereader) {
    barcodeReaderConfig = new BarcodeReader(barcodereader);
    barcodeDisplayer = new BarcodeDisplayer(barcodereader);
  }

  if (sharpness) {
    sharpnessControl = new SharpnessControl(sharpness, Roi);
  }
  whitebalance = pipeline->getWhiteBalance();

  if (whitebalance) {
    whiteBalanceControl = new WhiteBalanceControl(whitebalance, Roi);
  }
  photoTaker = new TakePhotos(&map);

  if (multifocus) {
    multifocusControl = new MultifocusControl(multifocus, Roi);
  }
  autoexposure = pipeline->getAutoexposure();
  if (autoexposure) {
    autoexposureControl =
        new AutoexposureControl(autoexposure, moduleControlConfig, Roi);
  }

  if (sensor->getColor() && pipeline->getColorSupport()) {
    toolbar = new ToolBar(pipeline);
  }

  glGenTextures(1, &videotex);
  glBindTexture(GL_TEXTURE_2D, videotex);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  initImGuiConfig();
}

Application::~Application() {
  if (autofocus) {
    delete autofocusConfig;
    delete autofocusControl;
  }
  delete pipeline;

  delete window;

  delete moduleControlConfig;
  delete moduleCtrl;
}

void Application::run() {
  printf("Running app\n");
  pipeline->setState(GST_STATE_PLAYING);

  pipeline->getVideoSize(&videoWidth, &videoHeight);
  Roi->setVideoSize(videoWidth, videoHeight);
  photoTaker->setImageSize(videoWidth, videoHeight);
  glClearColor(0.45f, 0.55f, 0.60f, 1.00f);

  start = std::chrono::high_resolution_clock::now();

  while (!window->shouldClose()) {
    glfwPollEvents();
    bool frame_created = createFrame();
    populateFrame();
    renderFrame();

    if (bufferNotFree.size() > 0) {
      int i = 0;
      while (i < bufferNotFree.size()) {
        if (GST_IS_BUFFER(bufferNotFree.at(i))) {
          gst_buffer_unmap(bufferNotFree.at(i), &(mapNotFree.at(i)));
          bufferNotFree.erase(std::next(bufferNotFree.begin(), i));
          mapNotFree.erase(std::next(mapNotFree.begin(), i));
        } else {
          i++;
        }
      }
    }
    if (frame_created) {
      if (GST_IS_BUFFER(videobuf)) {
        gst_buffer_unmap(videobuf, &map);
      } else {
        bufferNotFree.push_back(videobuf);
        mapNotFree.push_back(map);
      }
    }
  }
}

bool Application::createFrame() {
  GstSample *videosample = pipeline->getSample();
  /**
   * Load the frame into an openGL texture if a new one is available
   */
  bool created = false;
  if (videosample) {
    frameCounter++;
    videobuf = gst_sample_get_buffer(videosample);

    gst_buffer_map(videobuf, &map, GST_MAP_READ);
    gst_sample_unref(videosample);

    if (map.size == 8294400) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, videoWidth, videoHeight, 0,
                   GL_RGBA, GL_UNSIGNED_BYTE, map.data);
    } else if (map.size == 2073600) {
      glTexImage2D(GL_TEXTURE_2D, 0, 0x1909, videoWidth, videoHeight, 0, 0x1909,
                   GL_UNSIGNED_BYTE, map.data);
    }
    created = true;
  }

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();

  ImGui::NewFrame();
  return created;
}

void Application::populateFrame() {

  end = std::chrono::high_resolution_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  if (duration.count() > 1000000) {
    FPS = frameCounter * 1000000.0 / duration.count();
    frameCounter = 0;
    start = std::chrono::high_resolution_clock::now();
  }

  createDockSpace();

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 20.0f));

  if (ImGui::Begin("###DockSpace", nullptr, window_flags)) {
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
      ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
      ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    ImGuiWindowClass gstWindowClass;
    gstWindowClass.ClassId = ImGui::GetID("XXX");
    gstWindowClass.DockNodeFlagsOverrideSet =
        ImGuiDockNodeFlags_NoDockingOverMe;

    ImGui::SetNextWindowClass(&gstWindowClass);

    if (sensor->getColor() && pipeline->getColorSupport()) {
      toolbar->render();
    }
    ImGui::Begin("Gstreamer stream", nullptr,
                 ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoMove);
    if (freeze) {
      if (ImGui::Button((frozen) ? "Resume" : "Freeze")) {
        pipeline->freezeStream(!frozen);
        frozen = !frozen;
      }
    }
    ImGui::Text("Application frame rate : %d", (int)FPS);

    if (fpscounter) {
      int frame_rate_gstreamer;
      g_object_get(G_OBJECT(fpscounter), "framerate", &frame_rate_gstreamer,
                   NULL);

      ImGui::Text("Gstreamer frame rate  : %d", (int)frame_rate_gstreamer);
    }
    int T_line, T_wait;
    moduleCtrl->readReg(0x06, &T_line);

    moduleCtrl->readReg(0x08, &T_wait);

    int nb_lines, roi_1_height, roi_2_height, roi_1_subs_v, roi_2_subs_v;

    moduleCtrl->readReg(0x19, &roi_1_height);
    moduleCtrl->readReg(0x13, &roi_1_subs_v);
    moduleCtrl->readReg(0x18, &roi_2_height);
    moduleCtrl->readReg(0x1A, &roi_2_subs_v);

    int reg_dig_config_2;
    int Clamp_mode, Context, Trigger_margin;

    moduleCtrl->readReg(0x04, &reg_dig_config_2);
    Clamp_mode = reg_dig_config_2 & 0x1C;
    Context = reg_dig_config_2 & 0x100;
    Trigger_margin = reg_dig_config_2 & 0x60;

    nb_lines = roi_1_height / pow(2, roi_1_subs_v) +
               roi_2_height / pow(2, roi_2_subs_v) + Clamp_mode + Context +
               Trigger_margin;

    int fb_reg_frame;

    moduleCtrl->readReg(0x56, &fb_reg_frame);
    int frame_rate_limited_by_exposition = (fb_reg_frame * T_line) / 50;
    int frame_rate_limited_by_readout =
        (int((T_line) / ((float)50)) * nb_lines + T_wait);
    if (frame_rate_limited_by_readout < frame_rate_limited_by_exposition) {
      ImGui::Text("Sensor frame rate : %d",
                  (int)pow(10, 6) / frame_rate_limited_by_exposition);
    } else {
      ImGui::Text("Sensor frame rate : %d",
                  (int)pow(10, 6) / frame_rate_limited_by_readout);
    }

    /**
     *  Keep the video stream aspect ratio when drawing it to the screen
     */

    ImVec2 windowPosition = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();
    float windowSizeYOffset =
        (ImGui::GetCursorPosY()) + ImGui::GetStyle().FramePadding.y;
    windowSize.y -= windowSizeYOffset;
    ImVec2 streamSize;

    if (windowSize.y / videoHeight > windowSize.x / videoWidth) {
      streamSize.y = (windowSize.x * videoHeight) / videoWidth;
      streamSize.x = windowSize.x - 10;
    } else {
      streamSize.x = (windowSize.y * videoWidth) / videoHeight;
      streamSize.y = windowSize.y - 10;
    }

    ImVec2 streamPosition = ImVec2((windowSize.x - streamSize.x) * 0.5f,
                                   (windowSize.y - streamSize.y) * 0.5f);
    streamPosition.y += ImGui::GetCursorPosY();
    windowSize.y += windowSizeYOffset;
    ImGui::SetCursorPos(streamPosition);

    ImGui::Image((void *)(intptr_t)videotex, streamSize);

    photoTaker->render();

    ImDrawList *drawList = ImGui::GetWindowDrawList();

    Roi->render2(drawList, streamSize, windowPosition + streamPosition,
                 windowSize, windowPosition, focus_lost);

    if (autofocus) {
      focus_lost = autofocusControl->render(drawList, streamSize,
                                            windowPosition + streamPosition,
                                            windowSize, windowPosition);

      autofocusConfig->showWindow = true;

      autofocusConfig->security();
      autofocusConfig->render();
      autofocusDebug->render(autofocusControl->isAutofocusDone);
    }

    moduleControlConfig->showWindow = true;
    moduleControlConfig->render();

    if (whitebalance) {
      whiteBalanceControl->render();
    }

    if (sharpness) {
      sharpnessControl->render();
      if (autofocus) {

        if (autofocusControl->isAutofocusDone &&
            autofocusConfig->getStrategy() == 3) {
          sharpnessControl->plotAutofocus(autofocusDebug->getLogs());
        }
      }
    }

    if (autoexposure) {
      autoexposureControl->render();
    }
    if (multifocus) {
      multifocusControl->render();
    }

    if (barcodereader) {
      barcodeReaderConfig->render(drawList, streamSize,
                                  streamPosition + windowPosition);
      barcodeDisplayer->render();
    }

    sensor->render();

    ImGui::End();
  }

  ImGui::End();
}

void Application::renderFrame() {
  GLFWwindow *win = window->getWindow();

  ImGui::Render(); // render the window

  glfwMakeContextCurrent(win);
  glfwGetFramebufferSize(win, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwMakeContextCurrent(win);
  glfwSwapBuffers(win);
}

void Application::createDockSpace() {
  dockspace_flags = ImGuiDockNodeFlags_None;

  window_flags = ImGuiWindowFlags_NoDocking;

  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                  ImGuiWindowFlags_NoMove;
  window_flags |=
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

  if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    window_flags |= ImGuiWindowFlags_NoBackground;
}

void Application::initImGuiConfig() {
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigDockingWithShift = false;
  io.ConfigWindowsMoveFromTitleBarOnly = true;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(310, 150));
}

void Application::loadImGuiConfig() {
  std::string name = "imgui.ini";

  if (access(name.c_str(), 0) != 0) {
    std::ofstream flux;
    flux.open(name);
    flux << "[Window][Gstreamer stream]\n"
            "Pos=315,0\n"
            "Size=503,1005\n"
            "Collapsed=0\n"
            "DockId=0x00000003,0\n"
            "ClassId=0xB60BE08C\n"

            "[Window][Debug##Default]\n"
            "Pos=60,60\n"
            "Size=400,400\n"
            "Collapsed=0\n"

            "[Window][Take Photo]\n"
            "Pos=0,628\n"
            "Size=313,377\n"
            "Collapsed=0\n"
            "DockId=0x00000005,1\n"

            "[Window][Autofocus Controle]\n"
            "Pos=0,636\n"
            "Size=313,371\n"
            "Collapsed=0\n"
            "DockId=0x00000005,0\n"

            "[Window][Configuration]\n"
            "Pos=0,0\n"
            "Size=313,626\n"
            "Collapsed=0\n"
            "DockId=0x00000004,0\n"

            "[Window][ModuleControl]\n"
            "Pos=0,0\n"
            "Size=313,626\n"
            "Collapsed=0\n"
            "DockId=0x00000004,1\n"

            "[Window][Barcode Config]\n"
            "Pos=0,0\n"
            "Size=313,626\n"
            "Collapsed=0\n"
            "DockId=0x00000004,2\n"

            "[Window][Debug]\n"
            "Pos=0,628\n"
            "Size=313,377\n"
            "Collapsed=0\n"
            "DockId=0x00000005,2\n"

            "[Window][conf]\n"
            "Pos=60,60\n"
            "Size=170,117\n"
            "Collapsed=0\n"

            "[Window][###DockSpace]\n"
            "Pos=0,0\n"
            "Size=818,1005\n"
            "Collapsed=0\n"

            "[Window][Decoded barcodes]\n"
            "Pos=0,628\n"
            "Size=313,377\n"
            "Collapsed=0\n"
            "DockId=0x00000005,3\n"

            "[Window][Autofocus Control]\n"
            "Pos=0,628\n"
            "Size=313,377\n"
            "Collapsed=0\n"
            "DockId=0x00000005,0\n"

            "[Docking][Data]\n"
            "DockSpace     ID=0x5DAB574B Window=0xD327CEBC Pos=0,0 "
            "Size=818,1005 Split=X\n"
            "  DockNode    ID=0x00000002 Parent=0x5DAB574B SizeRef=313,1082 "
            "Split=Y\n"
            "    DockNode  ID=0x00000004 Parent=0x00000002 SizeRef=252,661 "
            "Selected=0x72D1FAD0\n"
            "    DockNode  ID=0x00000005 Parent=0x00000002 SizeRef=252,398 "
            "Selected=0x392A5ADD\n"
            "  DockNode    ID=0x00000003 Parent=0x5DAB574B SizeRef=381,1082 "
            "CentralNode=1 HiddenTabBar=1 Selected=0x213081DE\n";

    flux.close();
  }
}
