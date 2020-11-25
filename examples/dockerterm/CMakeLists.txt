set(SAMPLE_DOCKERTERM_HEADERS ${SAMPLE_DOCKERTERM_HEADERS}
)
set(SAMPLE_DOCKERTERM_SOURCES ${SAMPLE_DOCKERTERM_SOURCES}
    "dockerterm.cpp" 
)

add_executable(dockerterm ${EXAMPLE_HEADERS} ${EXAMPLE_SOURCES} ${SAMPLE_DOCKERTERM_HEADERS} ${SAMPLE_DOCKERTERM_SOURCES})
target_link_libraries(dockerterm PUBLIC SDL2::SDL2main SDL2::SDL2  imgui freetype HexeTerminal ImGuiTerminal OpenGL::GL GLEW::GLEW)
target_include_directories(dockerterm PRIVATE "${imgui_SOURCE_DIR}/examples/")
add_custom_command(TARGET dockerterm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${FONTS_DIRECTORY}"
        $<TARGET_FILE_DIR:dockerterm>
)
