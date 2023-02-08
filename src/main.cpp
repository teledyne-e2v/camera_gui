#include <iostream>

#include "Application.hpp"

int main(int argc, char **argv)
{
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
