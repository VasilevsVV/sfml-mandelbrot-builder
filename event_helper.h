#ifndef EVENT_CPP
#define EVENT_CPP

#include "main_loop_helper.h"

class EventHelper
{
    sf::RenderWindow *window;

  public:
    EventHelper(sf::RenderWindow *window);
    void processEvents(AppController *helper);
};

Rectangle<double> scaleCoordinates(Rectangle<double> initialCoords,
                                   Rectangle<unsigned int> imgCoorgs,
                                   Rectangle<unsigned int> newRegion);
#endif