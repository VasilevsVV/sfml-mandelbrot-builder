#ifndef MAIN_CPP
#define MAIN_CPP

#include "main_loop_helper.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1024, 720), "Mandelbrot");
    MainLoopHelper helper(&window);

    //Main loop
    helper.startMainLoop();

    return 0;
}

#endif
