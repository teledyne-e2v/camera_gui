#include "Application.hpp"
#include <unistd.h>
#include <fstream>
#include <chrono>
Application::Application(int argc, char **argv)
{
    loadImGuiConfig();

    moduleCtrl = new ModuleCtrl();

#ifndef DEBUG_MODE
    moduleCtrl->ModuleControlInit(); // init ic2
#endif

    window = new Window();

    pipeline = new Pipeline(argc, argv);

    moduleControlConfig = new ModuleControl(moduleCtrl);
    Roi = new ROI();
    freeze=pipeline->getImageFreeze();

    autofocus = pipeline->getAutofocus();
    if(autofocus)
    {
    autofocusConfig = new Config(autofocus);
    autofocusControl = new AutofocusControl(autofocus, moduleCtrl, moduleControlConfig, autofocusConfig, Roi);
    autofocusDebug = new Debug(autofocus);
    }
    barcodereader = pipeline->getBarcodeReader();
    if(barcodereader)
    {
    barcodeReaderConfig = new BarcodeReader(barcodereader);
    barcodeDisplayer = new BarcodeDisplayer(barcodereader);
    }

    photoTaker = new TakePhotos(&map);

    multifocus = pipeline->getMultifocus();
	if(multifocus)
{
    multifocusControl = new MultifocusControl(multifocus, Roi);
}
	autoexposure=pipeline->getAutoexposure();
	if(autoexposure)
{
    autoexposureControl = new AutoexposureControl(autoexposure, moduleControlConfig, Roi);
}

    glGenTextures(1, &videotex);
    glBindTexture(GL_TEXTURE_2D, videotex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    initImGuiConfig();
}

Application::~Application()
{
    if(autofocus)
    {
    delete autofocusConfig;
    delete autofocusControl;
    }
    delete pipeline;

    delete window;

    delete moduleControlConfig;
    delete moduleCtrl;
}

void Application::run()
{
    printf("Running app\n");
    pipeline->setState(GST_STATE_PLAYING);
    pipeline->getVideoSize(&videoWidth, &videoHeight);
    Roi->setVideoSize(videoWidth, videoHeight);
    photoTaker->setImageSize(videoWidth, videoHeight);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);

    while (!window->shouldClose())
    {
        glfwPollEvents();
        bool created = createFrame();
        populateFrame();
        renderFrame();

        if(created)
          gst_buffer_unmap(videobuf, &map);

    }
}

bool Application::createFrame()
{
    bool created = false;
    GstSample *videosample = pipeline->getSample();
    /**
     * Load the frame into an openGL texture if a new one is available
     */
    if (videosample)
    {
        videobuf = gst_sample_get_buffer(videosample);

        gst_buffer_map(videobuf, &map, GST_MAP_READ);
        gst_sample_unref(videosample);
        glTexImage2D(GL_TEXTURE_2D, 0, 0x1909, videoWidth, videoHeight, 0, 0x1909,
                     GL_UNSIGNED_BYTE, map.data);

        created = true;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    return created;
}

void Application::populateFrame()
{

    if (frameCounter == 0)
    {
        start = std::chrono::high_resolution_clock::now();
    }
    end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    if(duration.count() > 1000000)
    {
        frameCounter++;
        FPS=frameCounter*1000000.0/duration.count();
        frameCounter=0;
    }
    else
    {
        frameCounter++;
    }


    createDockSpace();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    if (ImGui::Begin("###DockSpace", nullptr, window_flags))
    {
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        ImGuiWindowClass gstWindowClass;
        gstWindowClass.ClassId = ImGui::GetID("XXX");
        gstWindowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoDockingOverMe;

        ImGui::SetNextWindowClass(&gstWindowClass);
        ImGui::Begin("Gstreamer stream", nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove);
	if(freeze)
{
        if (ImGui::Button((frozen) ? "Resume" : "Freeze"))
        {
            pipeline->freezeStream(!frozen);
            frozen = !frozen;
        }
}
        ImGui::Text("%d",(int)FPS);

        /**
         *  Keep the video stream aspect ratio when drawing it to the screen
         */

        ImVec2 windowPosition = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        float windowSizeYOffset = (ImGui::GetCursorPosY()) + ImGui::GetStyle().FramePadding.y;
        windowSize.y -= windowSizeYOffset;
        ImVec2 streamSize;

        if (windowSize.y / videoHeight > windowSize.x / videoWidth)
        {
            streamSize.y = (windowSize.x * videoHeight) / videoWidth;
            streamSize.x = windowSize.x - 10;
        }
        else
        {
            streamSize.x = (windowSize.y * videoWidth) / videoHeight;
            streamSize.y = windowSize.y - 10;
        }

        ImVec2 streamPosition = ImVec2((windowSize.x - streamSize.x) * 0.5f, (windowSize.y - streamSize.y) * 0.5f);
        streamPosition.y += ImGui::GetCursorPosY();
        windowSize.y += windowSizeYOffset;
        ImGui::SetCursorPos(streamPosition);
        ImGui::Image((void *)(intptr_t)videotex, streamSize);

        photoTaker->render();
        ImDrawList *drawList = ImGui::GetWindowDrawList();


	Roi->render2(drawList, streamSize, windowPosition + streamPosition, windowSize, windowPosition, focus_lost);

        if(autofocus)
    	{
        focus_lost = autofocusControl->render(drawList, streamSize, windowPosition + streamPosition, windowSize, windowPosition);

        autofocusConfig->showWindow = true;

        autofocusConfig->security();
        autofocusConfig->render();
	autofocusDebug->render(autofocusControl->isAutofocusDone);
        }
        moduleControlConfig->showWindow = true;
        moduleControlConfig->render();
	
	if(autoexposure)
{
        autoexposureControl->render();
}
if(multifocus)
{
        multifocusControl->render();
}
if(barcodereader)
{
        barcodeReaderConfig->render(drawList, streamSize, streamPosition + windowPosition);
        barcodeDisplayer->render();
}

        
        ImGui::End();
    }

    ImGui::End();
}

void Application::renderFrame()
{
    GLFWwindow *win = window->getWindow();

    ImGui::Render(); // render the window

    glfwMakeContextCurrent(win);
    glfwGetFramebufferSize(win, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwMakeContextCurrent(win);
    glfwSwapBuffers(win);
}

void Application::createDockSpace()
{
    dockspace_flags = ImGuiDockNodeFlags_None;

    window_flags = ImGuiWindowFlags_NoDocking;

    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;
}

void Application::initImGuiConfig()
{
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigDockingWithShift = false;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(310, 150));
}

void Application::loadImGuiConfig()
{
    std::string name = "imgui.ini";

    if (access(name.c_str(), 0) != 0)
    {
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
                "DockSpace     ID=0x5DAB574B Window=0xD327CEBC Pos=0,0 Size=818,1005 Split=X\n"
                "  DockNode    ID=0x00000002 Parent=0x5DAB574B SizeRef=313,1082 Split=Y\n"
                "    DockNode  ID=0x00000004 Parent=0x00000002 SizeRef=252,661 Selected=0x72D1FAD0\n"
                "    DockNode  ID=0x00000005 Parent=0x00000002 SizeRef=252,398 Selected=0x392A5ADD\n"
                "  DockNode    ID=0x00000003 Parent=0x5DAB574B SizeRef=381,1082 CentralNode=1 HiddenTabBar=1 Selected=0x213081DE\n";

        flux.close();
    }
}
