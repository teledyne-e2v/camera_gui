#include "Pipeline.hpp"


class ToolBar
{

private:
    Pipeline *pipeline;
    bool isColor=false;
    std::string pixels[4] = {"bggr","gbrg","grbg","rggb"};
    int selected_pixel = 3;
    int previous_selected_pixel = 3;
    int step =0;

public:
    ToolBar(Pipeline *pipeline);
    ~ToolBar();
    void render();
};
