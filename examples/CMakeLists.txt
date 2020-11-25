set(FONTS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/fonts)

set(EXAMPLE_HEADERS ${EXAMPLE_HEADERS}
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_freetype_ex.h"
    "${imgui_SOURCE_DIR}/examples/imgui_impl_opengl3.h"
    "${imgui_SOURCE_DIR}/examples//imgui_impl_sdl.h"
)
set(EXAMPLE_SOURCES ${EXAMPLE_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_freetype_ex.cpp"
    "${imgui_SOURCE_DIR}/examples/imgui_impl_opengl3.cpp"
    "${imgui_SOURCE_DIR}/examples/imgui_impl_sdl.cpp"
)

add_subdirectory(dockerterm)
add_subdirectory(simple)
add_subdirectory(terminal)