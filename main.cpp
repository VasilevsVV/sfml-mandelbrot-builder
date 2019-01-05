#include "renderer.h"

#include <SFML/Graphics.hpp>

#include <memory>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
    std::shared_ptr<sf::Image> image = std::make_shared<sf::Image>();
    image->create(window.getSize().x, window.getSize().y, sf::Color::Black);

    AsyncMandelbrotRenderer renderer(image);
    ImageCoords img { {0, 0}, window.getSize() };
    PaneCoords pane { {-2.0, 1.0}, {-1.5, 1.5} };

    sf::Texture texture;
    sf::Sprite sprite;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // render to image
        auto future = renderer.render_async(img, pane);

        // ...

        window.clear();
        future.wait();

        texture.update(*image);
        sprite.setTexture(texture);

        window.draw(sprite);
        window.display();
    }

    return 0;
}

