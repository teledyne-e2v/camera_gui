
#include "imgui.h"
#include "string.h"
#include "stdlib.h"
#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/app/app.h>
class Debug
{
private:
    char logs[20000];
    void updateLogs();
    GstElement *autofocus;

public:
    void render(bool autofocusTrigger);
    Debug(GstElement *autofocus);

    char *getLogs();
};
