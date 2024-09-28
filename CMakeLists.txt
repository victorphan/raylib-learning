cmake_minimum_required(VERSION 3.28)
project(template)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(raylib)
find_package(glm)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} raylib glm::glm)

add_executable(pong src/pong.cpp)
target_include_directories(pong PRIVATE include)
target_link_libraries(pong raylib glm::glm)

add_executable(snake src/snake.cpp)
target_include_directories(snake PRIVATE include assets)
target_link_libraries(snake raylib glm::glm)

add_executable(tetris src/tetris.cpp)
target_include_directories(tetris PRIVATE include assets)
target_link_libraries(tetris raylib glm::glm)