#ifndef HELPER_H
#define HELPER_H

#include <SFML/Graphics.hpp>

bool display_region_rect = false;
sf::RectangleShape region_selection_rect;

void display_auxiliary_entities(sf::RenderWindow *window);

#endif //HELPER_H