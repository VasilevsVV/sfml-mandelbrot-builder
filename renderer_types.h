#ifndef RENDERER_TYPES_H
#define RENDERER_TYPES_H

#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Color.hpp"

#include <functional>
#include <cassert>

//! Calculation helper functions
namespace calc {

// Return value scaled from [x_min, x_max] to [a,b]
template <typename T_in, typename T_out>
inline T_out scale(T_in x, T_in x_min, T_in x_max, T_out a, T_out b)
{
    assert(x_min < x_max);
    assert(a < b);
    return T_out((b - a) * (x - x_min) / (x_max - x_min)) + a;
}

// Linear Interpolation
template <typename T, typename U>
inline T lerp(T v0, T v1, U t)
{
    return T((1 - t) * v0 + t * v1);
}

// Linear Interpolation of color
inline sf::Color lerp_color(sf::Color a, sf::Color b, double factor)
{
    return {
        calc::lerp(a.r, b.r, factor),
        calc::lerp(a.g, b.g, factor),
        lerp(a.b, b.b, factor)};
}

inline sf::Vector2<double> scale_down_vector(sf::Vector2<double> init, sf::Vector2u startVect, sf::Vector2u endVect)
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

}

//! Rectangle type
template <typename T>
struct Rectangle
{
    sf::Vector2<T> topleft, bottomright;

    void scale_by(Rectangle<unsigned int> img,
                  Rectangle<unsigned int> reg) {
        Rectangle<double> res;
        auto x_vect = calc::scale_down_vector({topleft.x, bottomright.x},
                                            {img.topleft.x, img.bottomright.x},
                                            {reg.topleft.x, reg.bottomright.x});
        auto y_vect = calc::scale_down_vector({topleft.y, bottomright.y},
                                             {img.topleft.y, img.bottomright.y},
                                             {reg.topleft.y, reg.bottomright.y});
        topleft = {x_vect.x, y_vect.x};
        bottomright = {x_vect.y, y_vect.y};
    }
};

//!  PalleteFn: a callable with a signature of sf::Color(int i, int N)
//!             that is used to convert number of iterations to color.
//!             This should accept an arbitary non-negative value of i and N
//!             where i <= N. Otherwise behaviour is undefined.
using PaletteFn = std::function<sf::Color(const int, const int)>;

//! Sample Pallette functions
namespace palette
{
inline sf::Color simple(const int i, const int N)
{
    if (i != N)
    {
        return {0, 0, 0};
    }
    else
    {
        return {255, 255, 255};
    }
}

inline sf::Color grayscale(const int i, const int N)
{
    if (i != N)
    {
        return {calc::scale<int, sf::Uint8>(i, 0, N, 0, 255),
                calc::scale<int, sf::Uint8>(i, 0, N, 0, 255),
                calc::scale<int, sf::Uint8>(i, 0, N, 0, 255)};
    }
    else
    {
        return {0, 0, 0};
    }
}

inline sf::Color ultra_fractal(const int i, const int N)
{
    // Ultra Fractal palette
    const static std::array<sf::Color, 12> mapping{
        sf::Color{25, 7, 26},
        sf::Color{9, 1, 47},
        sf::Color{0, 7, 100},
        sf::Color{12, 44, 138},
        sf::Color{24, 82, 177},
        sf::Color{57, 125, 209},
        sf::Color{211, 236, 248},
        sf::Color{241, 233, 191},
        sf::Color{248, 201, 95},
        sf::Color{255, 170, 0},
        sf::Color{204, 128, 0},
        sf::Color{153, 87, 0}};

    const double value = calc::scale<int, double>(i, 0, N, 0, mapping.size());
    const int value_int = int(value);
    const int colormap_begin = value_int % (mapping.size() - 1);
    const double factor = value - value_int;

    return calc::lerp_color(mapping[colormap_begin],
                            mapping[colormap_begin + 1], factor);
}
} // namespace palette


#endif //RENDERER_TYPES_H
