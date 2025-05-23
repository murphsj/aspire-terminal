cmake_minimum_required(VERSION 3.16)
project(aspireTerminalPrototype VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_BUILD_TYPE "Debug")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

# TODO: build bfd with its dependencies if the static version can't be found
find_library (LIBBFD_LIBRARIES
  NAMES
    bfd
  PATHS
    ENV LIBRARY_PATH
    ENV LD_LIBRARY_PATH)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Test)
find_package(ZLIB REQUIRED)
qt_standard_project_setup()

file(GLOB SRCS "${CMAKE_SOURCE_DIR}/src/*.cpp")
qt_add_executable(
    aspireTerminalPrototype
    main.cpp
    ${SRCS}
)

target_link_libraries(aspireTerminalPrototype PRIVATE Qt::Widgets)
target_link_libraries(aspireTerminalPrototype PRIVATE Qt::Core)
target_link_libraries(aspireTerminalPrototype PRIVATE Qt::Test)
target_link_libraries(aspireTerminalPrototype PRIVATE ZLIB::ZLIB)
target_link_libraries(aspireTerminalPrototype PRIVATE ${LIBBFD_LIBRARIES})


set_target_properties(aspireTerminalPrototype PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)