#ifndef MAIN_CPP
#define MAIN_CPP

#include "main_loop_helper.h"
#include "event_helper.h"

#include "renderer.h"

int main()
{
    sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(800, 600), "SFML works!");

    using Renderer = AsyncMandelbrotRenderer<double>;
    using PaneCoords =  Renderer::PaneCoords;
    using ImageCoords = Renderer::ImageCoords;

    Renderer renderer;
    ImageCoords img { {0, 0}, window->getSize() };
    PaneCoords pane { {-2.25, -1.5}, { 0.75, 1.5 } };

    renderer.set_palette(palette::grayscale);

    // Texture
    sf::Texture texture;
    texture.create(window->getSize().x, window->getSize().y);

    sf::Sprite sprite(texture);

    // Initialization

    MainLoopHelper *helper = new MainLoopHelper(window);

    //Main loop

    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            processEvent(window, event, helper);
        }

        // render to image

        auto future = renderer.render_async(img, pane, 1000);
        window->clear();

        texture.update(future.get());

        window->draw(sprite);
        helper->displayAuxiliaryEntities();
        //window->draw(*region_selection_rect);
        window->display();
    }
    return 0;
}

#endif
