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
      : coords(std::move(cf.coords)), future(std::move(cf.future)) {}

public:
  chunkFuture(const sf::Vector2u &coords, std::future<sf::Image> &&future)
      : coords(coords), future(std::move(future)) {}
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

  static constexpr int iterationMagnificationCoeff = 128;
  int currentIterationCount;

  sf::RectangleShape regionSelectionRect;
  sf::RenderWindow *window;

  Renderer renderer;
  ImageCoords img;
  PaneCoords pane;

  std::stack<ZoomState> zoomStack;

  sf::Texture texture;
  sf::Sprite sprite;

  std::list<chunkFuture> chunksFutureList;

  void initializeAuxillaryEntities();
  std::list<imgChunk> splitImagesToChunks(const Rectangle<uint>& img,
                                          const Rectangle<double>& pane,
                                          uint split_factor);
  std::list<chunkFuture> runAsyncRender(const std::list<imgChunk>& chunkList);

  void addStateToStack(PaneCoords pane, bool update);

  bool zoomOut();
  bool maxZoomOut();
  bool cancelAllRender();

  void setDefaultValues();

public:
  MainLoopHelper(sf::RenderWindow *window);
  ~MainLoopHelper();

  void displayAuxiliaryEntities();
  void setDisplayRectStart(sf::Vector2f pos);
  void setDisplayRectEnd(sf::Vector2f pos);
  Rectangle<unsigned int> getSelectedRegion();
  void toggleRegionRect(bool state);
  bool isRegionDisplayed();

  void processFrame();
  void processEvents();

  void startMainLoop();
};

Rectangle<double> scaleCoordinates(const Rectangle<double>& initialCoords,
                                   const Rectangle<unsigned int>& imgCoorgs,
                                   const Rectangle<unsigned int>& newRegion);

#endif //HELPER_H
