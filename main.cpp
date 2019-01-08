#ifndef MAIN_CPP
#define MAIN_CPP

#include "main_loop_helper.h"

#include "renderer.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");

    AppController app(&window);

    //Main loop

    app.startMainLoop();

    return 0;
}

#endif
