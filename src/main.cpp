#include <iostream>

#include "Application.hpp"
#include <topaz_api.h>


int main(int argc, char **argv)
{
    char video[]="/dev/video0";
	initialization(video,2);
    try
    {
        Application *app = new Application(argc, argv);
        
        app->run();
        
        delete app;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}
