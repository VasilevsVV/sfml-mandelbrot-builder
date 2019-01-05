#ifndef RENDERER_H
#define RENDERER_H

#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Image.hpp"

#include <memory>
#include <future>

template <typename T>
struct Rectangle {
    sf::Vector2<T> topleft, bottomright;
};

using PaneCoordValue = double;
using ImageCoords = Rectangle<unsigned int>;
using PaneCoords = Rectangle<PaneCoordValue>;

//! Base renderer with Mandelbrot implementation
class BaseMandelbrotRenderer {
    std::shared_ptr<sf::Image> _image;

protected:
    BaseMandelbrotRenderer(const std::shared_ptr<sf::Image>& image) : _image(image) {}

    void render(const ImageCoords& image_coordinates,
                const PaneCoords& pane_coordinates) {
        BaseMandelbrotRenderer::render_fn(_image, image_coordinates, pane_coordinates);
    }

private:
    static void render_fn(std::shared_ptr<sf::Image> image,
                          const ImageCoords image_coordinates,
                          const PaneCoords pane_coordinates) {

    }
};


//! Renders requested task in an async thread
//!
//! render_async() returns a future object.
//! With it you can wait for execution to complete with std::future::wait()
//! or at the end of the scope.
class AsyncMandelbrotRenderer : public BaseMandelbrotRenderer {
public:
    AsyncMandelbrotRenderer(const std::shared_ptr<sf::Image>& image)
        : BaseMandelbrotRenderer(image) {}

    std::future<void> render_async(const ImageCoords& image_coordinates,
                                   const PaneCoords& pane_coordinates) {
        return std::async(std::launch::async,
                          &AsyncMandelbrotRenderer::render,
                          this,
                          image_coordinates,
                          pane_coordinates);
    }
};


#endif // RENDERER_H
