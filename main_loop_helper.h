#ifndef HELPER_H
#define HELPER_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <math.h>

class MainLoopHelper
{
    bool display_region_rect = true;
    sf::RectangleShape region_selection_rect;
    sf::RenderWindow *window;

    void initialize_auxiliary_entities();

  public:
    MainLoopHelper(sf::RenderWindow *window);
    ~MainLoopHelper();
    void displayAuxiliaryEntities();
    void setDisplayRectStart(sf::Vector2f pos);
    void setDisplayRectEnd(sf::Vector2f pos);
};

#endif //HELPER_H