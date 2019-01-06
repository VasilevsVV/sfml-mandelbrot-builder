#ifndef MAIN_CPP
#define MAIN_CPP

#include "main_loop_helper.h"

int main()
{
    sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(800, 600), "SFML works!");

    // Render variables
    std::shared_ptr<sf::Image> image = std::make_shared<sf::Image>();
    image->create(window->getSize().x, window->getSize().y, sf::Color::Black);
    AsyncMandelbrotRenderer renderer(image);
    ImageCoords img{{0, 0}, window->getSize()};
    PaneCoords pane{{-2.0, 1.0}, {-1.5, 1.5}};

    // Texture
    sf::Texture texture;
    texture.create(window->getSize().x, window->getSize().y);
    sf::Sprite sprite;

    // Initialization

    MainLoopHelper helper(window);

    //Main loop

    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
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
                    helper.togleRegionRect(true);
                    helper.setDisplayRectStart(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
                    helper.setDisplayRectEnd(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
                    break;
                case sf::Mouse::Right:
                    break;
                }
                break;
            case sf::Event::MouseButtonReleased:
                switch (event.key.code)
                {
                case sf::Mouse::Left:
                    helper.togleRegionRect(false);
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
                if (helper.isRegionDisplayed())
                    helper.setDisplayRectEnd(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
                break;
            }
        }

        // render to image
        auto future = renderer.render_async(img, pane);

        // ...

        window->clear();
        future.wait();

        texture.update(*image);
        sprite.setTexture(texture);

        window->draw(sprite);
        helper.displayAuxiliaryEntities();
        //window->draw(*region_selection_rect);
        window->display();
    }
    return 0;
}

#endif