#ifndef HELPER_H
#define HELPER_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <math.h>
#include "renderer.h"

class AppController
{
  bool display_region_rect = false;
  bool UpdateImage = true;
  sf::RectangleShape region_selection_rect;
  sf::RenderWindow *window;

  using Renderer = AsyncMandelbrotRenderer<double>;
  using PaneCoords = Renderer::PaneCoords;
  using ImageCoords = Renderer::ImageCoords;

  Renderer renderer;
  ImageCoords img;
  PaneCoords pane;

  sf::Texture texture;
  sf::Sprite sprite;

  std::future<sf::Image> future;

  void initialize_auxiliary_entities();

public:
  AppController(sf::RenderWindow *window);
  ~AppController();
  void displayAuxiliaryEntities();
  void setDisplayRectStart(sf::Vector2f pos);
  void setDisplayRectEnd(sf::Vector2f pos);
  Rectangle<unsigned int> getSelectedRegion();
  void togleRegionRect(bool state);
  void processFrame();
  void processEvents();
  void startMainLoop();

  bool isRegionDisplayed();
};

Rectangle<double> scaleCoordinates(Rectangle<double> initialCoords,
                                   Rectangle<unsigned int> imgCoorgs,
                                   Rectangle<unsigned int> newRegion);

#endif //HELPER_H
