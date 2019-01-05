#include "main_loop_helper.h"

void display_auxiliary_entities(sf::RenderWindow *window)
{
    window->draw(*(new sf::CircleShape(50.f)));
    region_selection_rect->setFillColor(sf::Color(0, 0, 0, 0));
    region_selection_rect->setOutlineThickness(5.f);
    region_selection_rect->setPosition({20, 20});
    region_selection_rect->setSize({50, 50});
    region_selection_rect->setOutlineColor(sf::Color::Yellow);

    window->draw(*region_selection_rect);
}