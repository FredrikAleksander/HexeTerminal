set(SAMPLE_TERMINAL_HEADERS ${SAMPLE_TERMINAL_HEADERS}
)
set(SAMPLE_TERMINAL_SOURCES ${SAMPLE_TERMINAL_SOURCES}
    "terminal.cpp" 
)

add_executable(terminal ${EXAMPLE_HEADERS} ${EXAMPLE_SOURCES} ${SAMPLE_TERMINAL_HEADERS} ${SAMPLE_TERMINAL_SOURCES})
target_link_libraries(terminal PUBLIC SDL2::SDL2main SDL2::SDL2 GLEW::GLEW imgui freetype HexeTerminal ImGuiTerminal)
target_include_directories(terminal PRIVATE "${imgui_SOURCE_DIR}/examples/")
add_custom_command(TARGET terminal POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${FONTS_DIRECTORY}"
        $<TARGET_FILE_DIR:terminal>
)