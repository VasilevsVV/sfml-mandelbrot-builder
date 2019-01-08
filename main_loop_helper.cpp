#include "main_loop_helper.h"

AppController::AppController(sf::RenderWindow *window)
{
    this->window = window;
    img = {{0, 0}, window->getSize()};
    pane = {{-2.0, -1.5}, {2.0, 1.5}};
    renderer.set_palette(palette::grayscale);
    texture.create(window->getSize().x, window->getSize().y);
    sprite.setTexture(texture);
    initialize_auxiliary_entities();
}

AppController::~AppController()
{
}

void AppController::initialize_auxiliary_entities()
{
    // Region rectangle initialization
    region_selection_rect.setFillColor(sf::Color(0, 0, 0, 0));
    region_selection_rect.setOutlineThickness(2.f);
    region_selection_rect.setPosition({20, 20});
    region_selection_rect.setSize({50, 50});
    region_selection_rect.setOutlineColor(sf::Color::Yellow);
}

void AppController::startMainLoop()
{
    while (window->isOpen())
    {
        processFrame();
    }
}

void AppController::processFrame()
{
    processEvents();

    // render to image
    // auto future = renderer.render_task_add(img, pane, 1000);
    if (UpdateImage)
    {
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

void AppController::processEvents()
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
                pane.scale_by(img, getSelectedRegion());
                UpdateImage = true;
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
                //setDisplayRectEnd(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
                auto pos = region_selection_rect.getPosition();
                setDisplayRectEnd(
                    sf::Vector2f(
                        event.mouseMove.x,
                        pos.y + (event.mouseMove.x - pos.x) * wRatio));
            }
            break;
        }
    }
}

void AppController::displayAuxiliaryEntities()
{
    if (display_region_rect)
        window->draw(region_selection_rect);
}

void AppController::setDisplayRectStart(sf::Vector2f pos)
{
    region_selection_rect.setPosition(pos);
}

void AppController::setDisplayRectEnd(sf::Vector2f pos)
{
    auto rectPos = region_selection_rect.getPosition();
    region_selection_rect.setSize({pos.x - rectPos.x, pos.y - rectPos.y});
}

Rectangle<unsigned int> AppController::getSelectedRegion()
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

void AppController::togleRegionRect(bool state)
{
    display_region_rect = state;
}

bool AppController::isRegionDisplayed()
{
    return display_region_rect;
}
