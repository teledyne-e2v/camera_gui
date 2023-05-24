#include "Pipeline.hpp"

class ToolBar
{

private:
    Pipeline *pipeline;
    bool isColor=false;
public:
    ToolBar(Pipeline *pipeline);
    ~ToolBar();
    void render();
};
