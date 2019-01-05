cmake_minimum_required(VERSION 2.6)

# Enable debug symbols by default
# equivalent to CMake "-D CMAKE_BUILD_TYPE=Debug" option
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

project(sfml-mandelbrot-builder)

# create target
set(EXECUTABLE_NAME mandelbrot-builder)
set(EXECUTABLE_SOURCE
    main.cpp)

add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_SOURCE})

# find and link SFML
find_package(SFML 2 COMPONENTS system window graphics REQUIRED)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()
