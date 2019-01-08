#ifndef HELPER_H
#define HELPER_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <math.h>
#include "renderer.h"
#include <list>
#include <stack>

using Renderer = AsyncMandelbrotRenderer<double>;
using PaneCoords = Renderer::PaneCoords;
using ImageCoords = Renderer::ImageCoords;

struct imgChunk
{
  Rectangle<double> pane;
  Rectangle<uint> img;
};

struct chunkFuture
{
  sf::Vector2u coords;
  std::future<sf::Image> future;

  chunkFuture(const chunkFuture &) = delete;
  chunkFuture(chunkFuture &&cf)
      : coords(std::move(cf.coords)), future(std::move(cf.future)){};

public:
  chunkFuture(const sf::Vector2u &coords, std::future<sf::Image> &&future)
      : future(std::move(future)), coords(coords) {}
};

struct ZoomState
{
  sf::Texture texture;
  PaneCoords pane;
  bool updateImage;
};

static sf::Color custom_pallet_from_i(const int i, const int N);
static sf::Color custom_pallet_relational(const int i, const int N);

class MainLoopHelper
{
  bool display_region_rect = false;
  bool UpdateImage = true;
  bool IsRenderFinished = true;
  sf::RectangleShape region_selection_rect;
  sf::RenderWindow *window;

  Renderer renderer;
  ImageCoords img;
  PaneCoords pane;

  std::stack<ZoomState> zoomStack;

  sf::Texture texture;
  sf::Sprite sprite;

  std::future<sf::Image> future;
  std::list<chunkFuture> chunks_futur_list;

  void initialize_auxiliary_entities();
  std::list<imgChunk> split_image_to_chunks(Rectangle<uint> img, Rectangle<double> pane, uint split_factor);
  std::list<chunkFuture> runAsyncRender(std::list<imgChunk> chunkList);

  void addStateToStack(PaneCoords pane, bool update);
  bool zoomOut();
  bool maxZoomOut();
  bool cancelAllRender();

public:
  MainLoopHelper(sf::RenderWindow *window);
  ~MainLoopHelper();
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
