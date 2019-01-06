#ifndef HELPER_H
#define HELPER_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <math.h>
#include "renderer.h"

class MainLoopHelper
{
    bool display_region_rect = false;
    sf::RectangleShape region_selection_rect;
    sf::RenderWindow *window;

    void initialize_auxiliary_entities();

  public:
    MainLoopHelper(sf::RenderWindow *window);
    ~MainLoopHelper();
    void displayAuxiliaryEntities();
    void setDisplayRectStart(sf::Vector2f pos);
    void setDisplayRectEnd(sf::Vector2f pos);
    Rectangle<float> getSelectedRegion();
    void togleRegionRect(bool state);

    bool isRegionDisplayed();
};

#endif //HELPER_H