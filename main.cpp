#ifndef MAIN_CPP
#define MAIN_CPP

#include "main_loop_helper.h"

#include "renderer.h"

int main()
{
    sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(1024, 720), "SFML works!");

    MainLoopHelper *helper = new MainLoopHelper(window);

    //Main loop

    helper->startMainLoop();

    return 0;
}

#endif
