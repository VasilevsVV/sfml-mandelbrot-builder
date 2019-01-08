#include "main_loop_helper.h"

MainLoopHelper::MainLoopHelper(sf::RenderWindow *window)
{
    this->window = window;
    img = {{0, 0}, window->getSize()};
    pane = {{-2.0, -1.5}, {2.0, 1.5}};
    //renderer.set_palette(palette::grayscale);
    //renderer.set_palette(custom_pallet_from_i);
    renderer.set_palette(custom_pallet_relational);
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
        UpdateImage = false;
        // future = renderer.render_async(img, pane, 1000);
        chunks_futur_list = runAsyncRender(split_image_to_chunks(img, pane, 6));
        IsRenderFinished = false;
    }

    // wait until timeout
    // future.wait_for(std::chrono::miliseconds(1000));
    // renderer.cancel_all();

    if (!IsRenderFinished)
    {
        bool renderState = true;
        for (auto it = chunks_futur_list.begin(); it != chunks_futur_list.end(); it++)
        {
            if (it->future.valid())
                if (it->future.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready)
                    texture.update(it->future.get(), it->coords.x, it->coords.y);
                else
                    renderState = false;
        }
        IsRenderFinished = renderState;
    }

    // if (future.valid() &&
    //     future.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready)
    //     texture.update(future.get());

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

std::list<chunkFuture> MainLoopHelper::runAsyncRender(std::list<imgChunk> chunkList)
{
    std::list<chunkFuture> resList;
    for (auto it = chunkList.begin(); it != chunkList.end(); it++)
    {
        resList.emplace_back(it->img.topleft, renderer.render_async(it->img, it->pane, 5000));
    }
    return resList;
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
    sf::Vector2<double> res;
    double dist = init.y - init.x;
    double startDist = startVect.y - startVect.x;
    double dx = (endVect.x - startVect.x) / startDist;
    double dy = (endVect.y - startVect.x) / startDist;
    res.x = init.x + dist * dx;
    res.y = init.x + dist * dy;
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

std::list<imgChunk> MainLoopHelper::split_image_to_chunks(Rectangle<uint> img,
                                                          Rectangle<double> pane,
                                                          uint split_factor)
{
    std::list<imgChunk> res;
    assert(split_factor > 0);
    sf::Vector2u imgT = img.topleft;
    sf::Vector2<double> paneT = pane.topleft;
    uint dx_img = img.bottomright.x - img.topleft.x;
    uint dy_img = img.bottomright.y - img.topleft.y;
    double dx_pane = pane.bottomright.x - pane.topleft.x;
    double dy_pane = pane.bottomright.y - pane.topleft.y;
    for (int i = 0; i < split_factor; i++)
    {
        for (int j = 0; j < split_factor; j++)
        {
            imgChunk chunk;
            chunk.img.topleft.x = imgT.x + dx_img * i / split_factor;
            chunk.img.topleft.y = imgT.y + dy_img * j / split_factor;
            chunk.img.bottomright.x = imgT.x + dx_img * (i + 1) / split_factor;
            chunk.img.bottomright.y = imgT.y + dy_img * (j + 1) / split_factor;

            chunk.pane.topleft.x = paneT.x + dx_pane * i / split_factor;
            chunk.pane.topleft.y = paneT.y + dy_pane * j / split_factor;
            chunk.pane.bottomright.x = paneT.x + dx_pane * (i + 1) / split_factor;
            chunk.pane.bottomright.y = paneT.y + dy_pane * (j + 1) / split_factor;
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
    pane = {{-2.0, -1.5},
            {2.0, 1.5}};
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
