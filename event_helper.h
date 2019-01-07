#ifndef EVENT_CPP
#define EVENT_CPP

#include "main_loop_helper.h"

void processEvent(sf::RenderWindow *window, sf::Event event, MainLoopHelper *helper);
Rectangle<double> scaleCoordinates(Rectangle<double> initialCoords,
                                   Rectangle<unsigned int> imgCoorgs,
                                   Rectangle<unsigned int> newRegion);

#endif