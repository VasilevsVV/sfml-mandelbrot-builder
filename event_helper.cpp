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

sf::Vector2<double> scaleDownVector(sf::Vector2<double> init, sf::Vector2u startVect, sf::Vector2u endVect)
{
    //std::printf("[vect] => %f :: %f\n", init.x, init.y);
    sf::Vector2<double> res;
    double dist = init.y - init.x;
    double startDist = startVect.y - startVect.x;
    //std::printf("startVect => %d :: %d \n", startVect.x, startVect.y);
    // std::printf("StartDist => %f\n", startDist);
    double dx = (endVect.x - startVect.x) / startDist;
    double dy = (endVect.y - startVect.x) / startDist;
    res.x = init.x + dist * dx;
    res.y = init.x + dist * dy;
    std::printf("%f : %f \n", res.x, res.y);
    return res;
}

Rectangle<double> scaleCoordinates(Rectangle<double> init,
                                   Rectangle<unsigned int> img,
                                   Rectangle<unsigned int> reg)
{
    Rectangle<double> res;
    auto xVect = scaleDownVector({init.topleft.x, init.bottomright.x},
                                 {img.topleft.x, img.bottomright.x},
                                 {reg.topleft.x, reg.bottomright.x});
    auto yVect = scaleDownVector({init.topleft.y, init.bottomright.y},
                                 {img.topleft.y, img.bottomright.y},
                                 {reg.topleft.y, reg.bottomright.y});
    res.topleft = {xVect.x, yVect.x};
    res.bottomright = {xVect.y, yVect.y};
    return res;
}