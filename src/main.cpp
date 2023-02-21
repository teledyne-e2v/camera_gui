#include <iostream>

#include "Application.hpp"
#include "api.hpp"


int main(int argc, char **argv)
{
	initialization("/dev/video0",2);
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
