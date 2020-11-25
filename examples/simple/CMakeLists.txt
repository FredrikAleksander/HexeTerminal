set(SAMPLE_SIMPLE_HEADERS ${SAMPLE_SIMPLE_HEADERS}
)
set(SAMPLE_SIMPLE_SOURCES ${SAMPLE_SIMPLE_SOURCES}
    "simple.cpp" 
)

add_executable(simple ${EXAMPLE_HEADERS} ${EXAMPLE_SOURCES} ${SAMPLE_SIMPLE_HEADERS} ${SAMPLE_SIMPLE_SOURCES})
target_link_libraries(simple PUBLIC SDL2::SDL2main SDL2::SDL2  imgui freetype HexeTerminal ImGuiTerminal OpenGL::GL GLEW::GLEW)
target_include_directories(simple PRIVATE "${imgui_SOURCE_DIR}/examples/")
add_custom_command(TARGET simple POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${FONTS_DIRECTORY}"
        $<TARGET_FILE_DIR:simple>
)
