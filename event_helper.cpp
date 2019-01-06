#include "event_helper.h"

void processEvent(sf::RenderWindow *window, sf::Event event, MainLoopHelper *helper)
{
    switch (event.type)
    {
    case sf::Event::Closed:
        window->close();
        break;
    case sf::Event::MouseButtonPressed:
        switch (event.key.code)
        {
        case sf::Mouse::Left:
            helper->togleRegionRect(true);
            helper->setDisplayRectStart(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
            helper->setDisplayRectEnd(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
            break;
        case sf::Mouse::Right:
            break;
        }
        break;
    case sf::Event::MouseButtonReleased:
        switch (event.key.code)
        {
        case sf::Mouse::Left:
            helper->togleRegionRect(false);
            // auto rect = helper.getSelectedRegion();  // To be used in future.
            break;
        case sf::Mouse::Right:
            break;
        }
        break;
    case sf::Event::KeyPressed:
        break;
    case sf::Event::KeyReleased:
        break;
    case sf::Event::MouseMoved:
        if (helper->isRegionDisplayed())
            helper->setDisplayRectEnd(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
        break;
    }
}