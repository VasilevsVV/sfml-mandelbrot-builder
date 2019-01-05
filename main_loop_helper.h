#ifndef HELPER_H
#define HELPER_H

#include <SFML/Graphics.hpp>
#include <memory>

static bool display_region_rect = true;
static std::shared_ptr<sf::RectangleShape> region_selection_rect = std::make_shared<sf::RectangleShape>();

void displayAuxiliaryEntities(sf::RenderWindow *window);
void setDisplayRectStart(sf::Vector2f pos);
void setDisplayRectEnd(sf::Vector2f pos);

#endif //HELPER_H