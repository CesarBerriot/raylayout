add_library(raylayout source/raylayout.c)
target_include_directories(raylayout PUBLIC include PRIVATE source)
target_link_libraries(raylayout PUBLIC raylib raygui PRIVATE hard_assert)