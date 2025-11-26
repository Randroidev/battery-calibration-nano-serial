add_library(displaylib
    src/PicoGraphics.cpp
    src/ST7789.cpp
    src/ST7735.cpp
)

target_include_directories(displaylib PUBLIC include)
