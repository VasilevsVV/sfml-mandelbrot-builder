#include "main_loop_helper.h"

MainLoopHelper::MainLoopHelper(sf::RenderWindow *window)
{
    this->window = window;
    setDefaultValues();

    initializeAuxillaryEntities();
}

MainLoopHelper::~MainLoopHelper()
{
}

void MainLoopHelper::setDefaultValues()
{
    //renderer.set_palette(palette::grayscale);
    //renderer.set_palette(palette::blue_wave);
    //renderer.set_palette(custom_pallet_from_i);
    //renderer.set_palette(custom_pallet_relational);
    renderer.set_palette(palette::ultra_fractal);

    img = {{0, 0}, window->getSize()};
    pane = {{-2.25, -1.5}, {1.5, 1.5}};

    texture.create(window->getSize().x, window->getSize().y);
    sprite.setTexture(texture);

    currentIterationCount = 2048;
}

void MainLoopHelper::initializeAuxillaryEntities()
{
    // Region rectangle initialization
    regionSelectionRect.setFillColor(sf::Color(0, 0, 0, 0));
    regionSelectionRect.setOutlineThickness(2.f);
    regionSelectionRect.setPosition({20, 20});
    regionSelectionRect.setSize({50, 50});
    regionSelectionRect.setOutlineColor(sf::Color::Yellow);
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

    if (UpdateImage)
    {
        UpdateImage = false;
        chunksFutureList = runAsyncRender(splitImagesToChunks(img, pane, 6));
        IsRenderFinished = false;

        sf::Image clear_image;
        clear_image.create(img.bottomright.x, img.bottomright.y);
        texture.update(clear_image);
    }

    if (!IsRenderFinished)
    {
        auto it = chunksFutureList.begin();
        while( it != chunksFutureList.end() )
        {
            if (it->future.wait_for(std::chrono::milliseconds(10))
                    == std::future_status::ready) {
                texture.update(it->future.get(), it->coords.x, it->coords.y);

                // remove completed future from the list
                it = chunksFutureList.erase(it);
            } else {
                ++it;
            }
        }
        IsRenderFinished = chunksFutureList.empty();
    }

    window->clear();
    window->draw(sprite);

    displayAuxiliaryEntities();

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
                toggleRegionRect(true);
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
                toggleRegionRect(false);
                addStateToStack(pane, !IsRenderFinished);
                pane = scaleCoordinates(pane, img, getSelectedRegion());
                cancelAllRender();
                UpdateImage = true;
                break;
            case sf::Mouse::Right:
                zoomOut();
                break;
            }
            break;
        case sf::Event::KeyPressed:
            break;
        case sf::Event::KeyReleased:
            switch (event.key.code)
            {
            case sf::Keyboard::Escape:
                if (maxZoomOut())
                    UpdateImage = true;
                break;
            }
            break;
        case sf::Event::MouseMoved:
            if (isRegionDisplayed())
            {
                //setDisplayRectEnd(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
                auto pos = regionSelectionRect.getPosition();
                setDisplayRectEnd(
                    sf::Vector2f(
                        event.mouseMove.x,
                        pos.y + (event.mouseMove.x - pos.x) * wRatio));
            }
            break;
        case sf::Event::Resized:
            maxZoomOut();
            break;
        }
    }
}

std::list<chunkFuture> MainLoopHelper::runAsyncRender(const std::list<imgChunk> &chunkList)
{
    std::list<chunkFuture> resList;
    for (const auto& chunk : chunkList)
    {
        resList.emplace_back(chunk.img.topleft,
                             renderer.render_async(chunk.img,
                                                   chunk.pane,
                                                   currentIterationCount));
    }
    return resList;
}

void MainLoopHelper::displayAuxiliaryEntities()
{
    if (display_region_rect)
        window->draw(regionSelectionRect);
}

void MainLoopHelper::setDisplayRectStart(sf::Vector2f pos)
{
    regionSelectionRect.setPosition(pos);
}

void MainLoopHelper::setDisplayRectEnd(sf::Vector2f pos)
{
    auto rectPos = regionSelectionRect.getPosition();
    regionSelectionRect.setSize({pos.x - rectPos.x, pos.y - rectPos.y});
}

// FIXME: handge 0-size and off-screen vector!
Rectangle<unsigned int> MainLoopHelper::getSelectedRegion()
{
    Rectangle<unsigned int> res;
    auto pos = regionSelectionRect.getPosition();
    auto size = regionSelectionRect.getSize();

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

void MainLoopHelper::toggleRegionRect(bool state)
{
    display_region_rect = state;
}

bool MainLoopHelper::isRegionDisplayed()
{
    return display_region_rect;
}

sf::Vector2<double> scaleDownVector(sf::Vector2<double> init, sf::Vector2u startVect, sf::Vector2u endVect)
{
    sf::Vector2<double> res;
    double dist = init.y - init.x;
    double startDist = startVect.y - startVect.x;
    double dx = (endVect.x - startVect.x) / startDist;
    double dy = (endVect.y - startVect.x) / startDist;
    res.x = init.x + dist * dx;
    res.y = init.x + dist * dy;
    return res;
}

Rectangle<double> scaleCoordinates(const Rectangle<double>& init,
                                   const Rectangle<unsigned int>& img,
                                   const Rectangle<unsigned int>& reg)
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

std::list<imgChunk> MainLoopHelper::splitImagesToChunks(const Rectangle<uint>& img,
                                                        const Rectangle<double>& pane,
                                                        uint split_factor)
{
    std::list<imgChunk> res;
    assert(split_factor > 0);

    uint dx_img = img.bottomright.x - img.topleft.x;
    uint dy_img = img.bottomright.y - img.topleft.y;

    double dx_pane = pane.bottomright.x - pane.topleft.x;
    double dy_pane = pane.bottomright.y - pane.topleft.y;

    uint split_width = dx_img / split_factor;
    uint split_height = dy_img / split_factor;

    double pane_split_width = dx_pane / double(split_factor);
    double pane_split_height = dy_pane / double(split_factor);

    imgChunk chunk;
    for (int i = 0; i < split_factor; i++)
    {
        for (int j = 0; j < split_factor; j++)
        {
            if( i == 0 ) {
                chunk.img.topleft.x = img.topleft.x;
                chunk.pane.topleft.x = pane.topleft.x;
            } else {
                chunk.img.topleft.x = img.topleft.x + split_width * i;
                chunk.pane.topleft.x = pane.topleft.x + pane_split_width * i;
            }

            if( j == 0 ) {
                chunk.img.topleft.y = img.topleft.y;
                chunk.pane.topleft.y = pane.topleft.y;
            } else {
                chunk.img.topleft.y = img.topleft.y + split_height * j;
                chunk.pane.topleft.y = pane.topleft.y + pane_split_height * j;
            }

            if( i == split_factor ) {
                chunk.img.bottomright.x = img.bottomright.x;
                chunk.pane.bottomright.x = pane.bottomright.x;
            } else {
                chunk.img.bottomright.x = chunk.img.topleft.x + split_width;
                chunk.pane.bottomright.x = chunk.pane.topleft.x + pane_split_width;
            }

            if( j == split_factor ) {
                chunk.img.bottomright.y = img.bottomright.y;
                chunk.pane.bottomright.y = pane.bottomright.y;
            } else {
                chunk.img.bottomright.y = chunk.img.topleft.y + split_height;
                chunk.pane.bottomright.y = chunk.pane.topleft.y + pane_split_height;
            }

            res.push_back(chunk);
        }
    }

    return res;
}

void MainLoopHelper::addStateToStack(PaneCoords pane, bool update)
{
    zoomStack.push({texture, pane, update});
}

bool MainLoopHelper::zoomOut()
{
    if (!zoomStack.empty())
    {
        pane = zoomStack.top().pane;
        texture = zoomStack.top().texture;
        UpdateImage = zoomStack.top().updateImage;
        zoomStack.pop();
        cancelAllRender();
        return true;
    }
    return false;
}

bool MainLoopHelper::maxZoomOut()
{
    bool res = false;
    while (!zoomStack.empty())
    {
        zoomStack.pop();
        res = true;
        cancelAllRender();
    }
    setDefaultValues();
    return res;
}

bool MainLoopHelper::cancelAllRender()
{
    renderer.cancel_all();
}

static sf::Color custom_pallet_from_i(const int i, const int N)
{
    if (i == N)
        return {0, 0, 0};
    return {sf::Uint8(i * 3),
            sf::Uint8(i * 2),
            sf::Uint8(i)};
}

static sf::Color custom_pallet_relational(const int i, const int N)
{
    if (i == N)
        return {0, 0, 0};
    return {sf::Uint8(255 * i / N),
            sf::Uint8(255 * i / N * 2),
            sf::Uint8(255 * i / N * 3)};
}
