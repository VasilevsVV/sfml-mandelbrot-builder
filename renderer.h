#ifndef RENDERER_H
#define RENDERER_H

#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Image.hpp"

#include <memory>
#include <future>
#include <cassert>
#include <thread>
#include <atomic>

#include "ctpl_stl.h"

namespace {
// Return value scaled from [x_min, x_max] to [a,b]
template <typename T_in, typename T_out>
inline T_out scale(T_in x, T_in x_min, T_in x_max, T_out a, T_out b)
{
    assert(x_min < x_max);
    assert(a < b);
    return T_out((b - a) * (x - x_min) / (x_max - x_min)) + a;
}

// Return value scaled from [x_min, x_max] to [a,b] in reverse
template <typename T_in, typename T_out>
inline T_out inverse_scale(T_in x, T_in x_min, T_in x_max, T_out a, T_out b) {
    return a - scale(x, x_min, x_max, a, b);
}

template <typename T, typename U, typename R>
inline R lerp(T v0, T v1, U t) {
    return R(U(1 - t) * U(v0) + t * U(v1));
}

// Linear Interpolation
template <typename T, typename U>
inline T lerp(T v0, T v1, U t) {
    return lerp<T,U,T>(v0, v1, t);
}
}

template <typename T>
struct Rectangle {
    sf::Vector2<T> topleft, bottomright;
};

//!  PalleteFn: a callable with a signature of sf::Color(int i, int N)
//!             that is used to convert number of iterations to color.
//!             This should accept an arbitary non-negative value of i and N
//!             where i <= N. Otherwise behaviour is undefined.
using PaletteFn = std::function<sf::Color(const int, const int)>;

//! Sample Pallette functions
namespace palette {
namespace helper {
inline sf::Color lerp_color(sf::Color a, sf::Color b, double factor) {
    return {
        lerp<sf::Uint8, double, sf::Uint8>(a.r, b.r, factor),
        lerp<sf::Uint8, double, sf::Uint8>(a.g, b.g, factor),
        lerp<sf::Uint8, double, sf::Uint8>(a.b, b.b, factor)
    };
}
}
    inline sf::Color simple(const int i, const int N) {
        if(i != N) {
            return {0,0,0};
        } else {
            return {255,255,255};
        }
    }

    inline sf::Color grayscale(const int i, const int N) {
        if( i != N ){
            return { ::scale<int, sf::Uint8>(i, 0, N, 0, 255),
                     ::scale<int, sf::Uint8>(i, 0, N, 0, 255),
                     ::scale<int, sf::Uint8>(i, 0, N, 0, 255) };
        } else {
            return {0,0,0};
        }

    }

    inline sf::Color ultra_fractal(const int i, const int N) {
        // Ultra Fractal palette
        constexpr int palette_size = 13;//16;
        const static std::array<sf::Color, palette_size> mapping {
           sf::Color{0,0,0},
           sf::Color{ 25,  7, 26}, // dark violett
           sf::Color{ 9 ,  1, 47}, // darkest blue
           sf::Color{ 4 ,  4, 73}, // blue 5
           sf::Color{ 0 ,  7,100}, // blue 4
           sf::Color{ 12, 44,138}, // blue 3
           sf::Color{ 24, 82,177}, // blue 2
           sf::Color{ 57,125,209}, // blue 1
           sf::Color{134,181,229}, // blue 0
           sf::Color{211,236,248}, // lightest blue
           sf::Color{241,233,191}, // lightest yellow
           sf::Color{248,201, 95}, // light yellow
           sf::Color{255,170,  0},   // dirty yellow
        };

        if(i == N) {
            return {0,0,0};
        }

        const double value = i / double(palette_size);
        const int value_int = int(value);
        const int colormap_begin = value_int % (palette_size);
        const double factor = value - value_int;

        if(colormap_begin < palette_size - 1) {
            return helper::lerp_color(mapping[colormap_begin],
                                      mapping[colormap_begin + 1], factor);
        } else {
            return helper::lerp_color(mapping[colormap_begin],
                                      mapping[0], factor);
        }
    }
}

//! Base renderer with Mandelbrot implementation
//!
//! Template parameters
//!  - PaneCoordT: type of value in Mandelbrot function space.
//!                Default is double;
template <typename PaneCoordT>
class BaseMandelbrotRenderer
{
    PaletteFn _palette;

public:
    using ImageCoords = Rectangle<unsigned int>;
    using PaneCoords = Rectangle<PaneCoordT>;

    void set_palette(const PaletteFn& palette) {
        _palette = palette;
    }

    // TODO: switch to vertex
    sf::Image render_simple(const ImageCoords& world_coordinates,
                            const Rectangle<PaneCoordT>& pane_coordinates,
                            int N = 10000) {
        enable_all();
        return render_fn(world_coordinates,
                         pane_coordinates,
                         N);
    }

    // cancel the rendering threads
    void cancel_all() {
        _is_rendering = false;
    }

    // enable the rendering threads
    void enable_all() {
        _is_rendering = true;
    }

protected:
    BaseMandelbrotRenderer(const PaletteFn& palette)
        : _palette(palette), _is_rendering(true) {}

    // Calculate the color of (x,y) point of the fractal
    sf::Color get_color_for_coord(PaneCoordT x, PaneCoordT y, int N) {
        PaneCoordT var_x = 0, var_y = 0;
        PaneCoordT var_x_squared = 0, var_y_squared = 0;

        int i = 0;
        while( var_x_squared + var_y_squared < 4 && i < N) {
            auto x_temp = var_x_squared - var_y_squared + x;
            auto y_temp = 2 * var_x * var_y + y;

            if (var_x == x_temp && var_y == y_temp) {
                  i = N;
                  break;
            }

            var_x = x_temp;
            var_y = y_temp;
            var_x_squared = var_x * var_x;
            var_y_squared = var_y * var_y;

            ++i;
        }

        return _palette(i, N);
    }

    // used to stop the render from another thread
    std::atomic<bool> _is_rendering;

    // Render into separate image
    // so we can sync the drawing to the shared image
    // TODO: perfomance improvements?
    sf::Image render_fn(const ImageCoords world_coord,
                        const PaneCoords pane_coord,
                        int N)
    {
        const auto img_min_x = world_coord.topleft.x;
        const auto img_max_x = world_coord.bottomright.x;
        const auto img_min_y = world_coord.topleft.y;
        const auto img_max_y = world_coord.bottomright.y;
        const auto pane_min_x = pane_coord.topleft.x;
        const auto pane_max_x = pane_coord.bottomright.x;
        const auto pane_min_y = pane_coord.topleft.y;
        const auto pane_max_y = pane_coord.bottomright.y;

        sf::Image new_image;
        new_image.create(img_max_x - img_min_x,
                         img_max_y - img_min_y);

        for(auto world_img_x = img_min_x;
            world_img_x < img_max_x; ++world_img_x)
        {
            for(auto world_img_y = img_min_y;
                world_img_y < img_max_y; ++world_img_y)
            {
                // set mandelbrot value
                PaneCoordT pane_x = ::scale(world_img_x, img_min_x, img_max_x,
                                            pane_min_x, pane_max_x);
                PaneCoordT pane_y = ::scale(world_img_y, img_min_y, img_max_y,
                                            pane_min_y, pane_max_y);

                sf::Color color = get_color_for_coord(pane_x, pane_y, N);
                new_image.setPixel(world_img_x - img_min_x,
                                   world_img_y - img_min_y,
                                   color);
            }

            // save some unnesessary checks
            if(!_is_rendering) {
                sf::Image blank_image;
                new_image.create(img_max_x - img_min_x,
                                 img_max_y - img_min_y);

                return blank_image;
            }
        }

        return new_image;
    }
};

//! Renders requested task in an async thread
//!
//! render_async() returns a future object.
//! With it you can wait for execution to complete with std::future::wait()
//! or at the end of the scope and get the image with std::future::get().
//!
//! This allows for the most simple parallel execution.
template <typename PaneCoordT>
class AsyncMandelbrotRenderer : public BaseMandelbrotRenderer<PaneCoordT>
{
    using Base = BaseMandelbrotRenderer<PaneCoordT>;

public:
    using ImageCoords = typename Base::ImageCoords;
    using PaneCoords = typename Base::PaneCoords;

    AsyncMandelbrotRenderer(const PaletteFn& palette = ::palette::simple)
        : Base(palette) {}

    std::future<sf::Image> render_async(const ImageCoords& image_coordinates,
                                   const PaneCoords& pane_coordinates,
                                   int N) {
        return std::async(std::launch::async,
                          &AsyncMandelbrotRenderer::render_simple,
                          this,
                          image_coordinates,
                          pane_coordinates,
                          N);
    }
};


//! Renders requested task in a thread pool.
//!
//! render_add_task() returns a future object.
//! With it you can wait for execution to complete with std::future::wait()
//! or at the end of the scope and get the image with std::future::get().
//!
//! Thread pool allows to eliminate the cost of creating new threads
//! every call while still executing in parallel.
//!
//! NOTE: this is very slow in debug!
template <typename PaneCoordT>
class ThreadedMandelbrotRenderer
        : public BaseMandelbrotRenderer<PaneCoordT>
{
    using Base = BaseMandelbrotRenderer<PaneCoordT>;
    static constexpr auto n_threads = 8;

    ctpl::thread_pool _pool;

public:
    using ImageCoords = typename Base::ImageCoords;
    using PaneCoords = typename Base::PaneCoords;

    ThreadedMandelbrotRenderer(const PaletteFn& palette = ::palette::simple)
        : Base(palette), _pool(n_threads) {}

    // Add task to the thread pool
    auto render_task_add(const ImageCoords& image_coordinates,
                        const PaneCoords& pane_coordinates,
                        int N) {
        Base::enable_all();
        return _pool.push([&] (int /* id */) {
            return this->render_fn(image_coordinates,
                                   pane_coordinates,
                                   N); });
    }

    // Wait for all computing threads to finish and stop all threads
    // if wait is true waits  for all remaining tasks to complete
    // otherwise the queue is cleared without running them
    void stop_all(bool wait = true) {
        _pool.stop(wait);
    }
};

#endif // RENDERER_H
