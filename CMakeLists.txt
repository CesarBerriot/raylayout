cmake_minimum_required(VERSION 3.31)
project(raylayout C)

include(FetchContent)
FetchContent_Declare(hard_assert GIT_REPOSITORY https://github.com/CesarBerriot/hard_assert.git GIT_TAG 1.2.1)
FetchContent_Declare(raygui GIT_REPOSITORY https://github.com/CesarBerriot/raygui.git GIT_TAG 4.0)
FetchContent_MakeAvailable(hard_assert raygui)

add_subdirectory(lib)
add_subdirectory(demo)