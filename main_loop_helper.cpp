#include "main_loop_helper.h"

MainLoopHelper::MainLoopHelper(sf::RenderWindow *window)
{
    this->window = window;
    initialize_auxiliary_entities();
}

MainLoopHelper::~MainLoopHelper()
{
}

void MainLoopHelper::initialize_auxiliary_entities()
{
    // Region rectangle initialization
    region_selection_rect.setFillColor(sf::Color(0, 0, 0, 0));
    region_selection_rect.setOutlineThickness(2.f);
    region_selection_rect.setPosition({20, 20});
    region_selection_rect.setSize({50, 50});
    region_selection_rect.setOutlineColor(sf::Color::Yellow);
}

void MainLoopHelper::displayAuxiliaryEntities()
{
    if (display_region_rect)
        window->draw(region_selection_rect);
}

void MainLoopHelper::setDisplayRectStart(sf::Vector2f pos)
{
    region_selection_rect.setPosition(pos);
}

void MainLoopHelper::setDisplayRectEnd(sf::Vector2f pos)
{
    auto rectPos = region_selection_rect.getPosition();
    region_selection_rect.setSize({pos.x - rectPos.x, pos.y - rectPos.y});
}