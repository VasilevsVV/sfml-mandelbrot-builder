#ifndef MAIN_CPP
#define MAIN_CPP

#include "renderer.h"
#include "main_loop_helper.h"

#include <SFML/Graphics.hpp>

#include <memory>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");

    // Render variables
    std::shared_ptr<sf::Image> image = std::make_shared<sf::Image>();
    image->create(window.getSize().x, window.getSize().y, sf::Color::Black);
    AsyncMandelbrotRenderer renderer(image);
    ImageCoords img{{0, 0}, window.getSize()};
    PaneCoords pane{{-2.0, 1.0}, {-1.5, 1.5}};

    // Texture
    sf::Texture texture;
    texture.create(window.getSize().x, window.getSize().y);
    sf::Sprite sprite;

    //Main loop

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseButtonPressed:
                break;
            case sf::Event::MouseButtonReleased:
                break;
            case sf::Event::KeyPressed:
                break;
            case sf::Event::KeyReleased:
                break;
            }
        }

        // render to image
        auto future = renderer.render_async(img, pane);

        // ...

        display_auxiliary_entities(&window);

        window.clear();
        future.wait();

        texture.update(*image);
        sprite.setTexture(texture);

        window.draw(sprite);
        //window.draw(region_selection_rect);
        window.display();
    }
    return 0;
}

#endif