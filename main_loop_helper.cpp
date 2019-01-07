#include "main_loop_helper.h"

MainLoopHelper::MainLoopHelper(sf::RenderWindow *window)
{
    this->window = window;
    img = {{0, 0}, window->getSize()};
    pane = {{-2.0, -1.5}, {2.0, 1.5}};
    renderer.set_palette(palette::grayscale);
    texture.create(window->getSize().x, window->getSize().y);
    sprite.setTexture(texture);
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

void MainLoopHelper::startMainLoop()
{
    while (window->isOpen())
    {
        processFrame();
    }
}

void MainLoopHelper::processFrame()
{
    processEvents();

    // render to image
    // auto future = renderer.render_task_add(img, pane, 1000);
    if (UpdateImage)
    {
        //pane = scaleCoordinates(pane, img, {{200, 150}, {600, 450}});
        UpdateImage = false;
        future = renderer.render_async(img, pane, 1000);
    }

    // wait until timeout
    // future.wait_for(std::chrono::miliseconds(1000));
    // renderer.cancel_all();

    if (future.valid() &&
        future.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready)
        texture.update(future.get());

    window->clear();
    window->draw(sprite);
    displayAuxiliaryEntities();
    //window->draw(*region_selection_rect);
    window->display();
}

void MainLoopHelper::processEvents()
{
    sf::Event event;

    while (window->pollEvent(event))
    {
        auto wSize = window->getSize();
        double wRatio = wSize.y / double(wSize.x);
        switch (event.type)
        {
        case sf::Event::Closed:
            window->close();
            break;
        case sf::Event::MouseButtonPressed:
            switch (event.key.code)
            {
            case sf::Mouse::Left:
                togleRegionRect(true);
                setDisplayRectStart(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
                setDisplayRectEnd(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
                break;
            case sf::Mouse::Right:
                break;
            }
            break;
        case sf::Event::MouseButtonReleased:
            switch (event.key.code)
            {
            case sf::Mouse::Left:
                togleRegionRect(false);
                pane = pane = scaleCoordinates(pane, img, getSelectedRegion());
                UpdateImage = true;
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
            if (isRegionDisplayed())
            {
                setDisplayRectEnd(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
            }
            break;
        }
    }
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

Rectangle<unsigned int> MainLoopHelper::getSelectedRegion()
{
    Rectangle<unsigned int> res;
    auto pos = region_selection_rect.getPosition();
    auto size = region_selection_rect.getSize();
    if (size.x >= 0)
    {
        res.topleft.x = pos.x;
        res.bottomright.x = pos.x + size.x;
    }
    else
    {
        res.topleft.x = pos.x + size.x;
        res.bottomright.x = pos.x;
    }

    if (size.y >= 0)
    {
        res.topleft.y = pos.y;
        res.bottomright.y = pos.y + size.y;
    }
    else
    {
        res.topleft.y = pos.y + size.y;
        res.bottomright.y = pos.y;
    }
    return res;
}

void MainLoopHelper::togleRegionRect(bool state)
{
    display_region_rect = state;
}

bool MainLoopHelper::isRegionDisplayed()
{
    return display_region_rect;
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