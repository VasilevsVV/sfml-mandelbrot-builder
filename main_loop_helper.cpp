#ifndef HELPER_C
#define HELPER_C

#include "main_loop_helper.h"

void display_auxiliary_entities(sf::RenderWindow *window)
{
    if (display_region_rect)
        window->draw(region_selection_rect);
}

#endif