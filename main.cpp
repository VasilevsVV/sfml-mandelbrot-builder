#ifndef MAIN_CPP
#define MAIN_CPP

#include "main_loop_helper.h"
#include "event_helper.h"

#include "renderer.h"

int main()
{
    sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(800, 600), "SFML works!");

    // using Renderer = ThreadedMandelbrotRenderer<double>;
    using Renderer = AsyncMandelbrotRenderer<double>;
    using PaneCoords = Renderer::PaneCoords;
    using ImageCoords = Renderer::ImageCoords;

    Renderer renderer;
    ImageCoords img{{0, 0}, window->getSize()};
    PaneCoords pane{{-2.0, -1.5}, {2.0, 1.5}};

    renderer.set_palette(palette::grayscale);

    // Texture
    sf::Texture texture;
    texture.create(window->getSize().x, window->getSize().y);

    sf::Sprite sprite(texture);

    // Initialization

    MainLoopHelper *helper = new MainLoopHelper(window);
    bool UpdateImage = true;
    std::future<sf::Image> future;

    //Main loop

    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            processEvent(window, event, helper);
        }

        // render to image
        // auto future = renderer.render_task_add(img, pane, 1000);
        if (UpdateImage)
        {
            pane = scaleCoordinates(pane, img, {{200, 150}, {600, 450}});
            UpdateImage = false;
            future = renderer.render_async(img, pane, 1000);
        }

        // wait until timeout
        // future.wait_for(std::chrono::miliseconds(1000));
        // renderer.cancel_all();
        if (future.valid())
            texture.update(future.get());

        window->clear();
        window->draw(sprite);
        helper->displayAuxiliaryEntities();
        //window->draw(*region_selection_rect);
        window->display();
    }
    return 0;
}

#endif
