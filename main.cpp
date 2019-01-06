#include "renderer.h"

#include <SFML/Graphics.hpp>

#include <memory>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Mandelbrot works!");

    using Renderer = AsyncMandelbrotRenderer<double>;
    using PaneCoords =  Renderer::PaneCoords;
    using ImageCoords = Renderer::ImageCoords;

    Renderer renderer;
    ImageCoords img { {0, 0}, window.getSize() };
    PaneCoords pane { {-2.25, -1.5}, { 0.75, 1.5 } };

    renderer.set_palette(palette::grayscale);

    sf::Texture texture;
    texture.create(window.getSize().x, window.getSize().y);

    sf::Sprite sprite(texture);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // render to image
        auto future = renderer.render_async(img, pane, 1000);
        window.clear();

        texture.update(future.get());

        window.draw(sprite);
        window.display();
    }

    return 0;
}

