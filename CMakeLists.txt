cmake_policy(SET CMP0074 NEW)
project(HexeTerminal)
cmake_minimum_required(VERSION 3.15)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

Include(FetchContent)

find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)

find_package(PNG)

FetchContent_Declare(
	freetype
	GIT_REPOSITORY https://github.com/freetype/freetype2.git
	GIT_TAG		   VER-2-10-2
)

FetchContent_GetProperties(freetype)
if(NOT freetype_POPULATED)
	FetchContent_Populate(freetype)
	add_subdirectory(${freetype_SOURCE_DIR} ${freetype_BINARY_DIR})
endif()

FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG		   v1.79
)

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
	FetchContent_Populate(imgui)
    #add_subdirectory(${imgui_SOURCE_DIR} ${imgui_BINARY_DIR})
    
    set(IMGUI_HEADERS ${IMGUI_HEADERS}
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui_config.h"
        "${imgui_SOURCE_DIR}/imgui_internal.h"
        "${imgui_SOURCE_DIR}/imgui.h"
        "${imgui_SOURCE_DIR}/imstb_rectpack.h"
        "${imgui_SOURCE_DIR}/imstb_textedit.h"
        "${imgui_SOURCE_DIR}/imstb_truetype.h")
    set(IMGUI_SOURCES ${IMGUI_SOURCES}
        "${imgui_SOURCE_DIR}/imgui_demo.cpp"
        "${imgui_SOURCE_DIR}/imgui_draw.cpp"
        "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
        "${imgui_SOURCE_DIR}/imgui.cpp"
    )

    add_library(imgui ${IMGUI_HEADERS} ${IMGUI_SOURCES})
    target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
    target_compile_definitions(imgui PUBLIC IMGUI_USER_CONFIG="${CMAKE_CURRENT_SOURCE_DIR}/imgui_config.h")
endif()

set(HEXE_TERMINAL_HEADERS ${HEXE_TERMINAL_HEADERS}
    "include/Hexe/System/Process.h"
    "include/Hexe/Terminal/Boxdraw.h"
    "include/Hexe/Terminal/PseudoTerminal.h"
    "include/Hexe/Terminal/TerminalDisplay.h"
    "include/Hexe/Terminal/TerminalEmulator.h"
    "include/Hexe/Terminal/Types.h"

    "src/boxdraw_data.h"
    "src/config.def.h"
    "src/ImGuiTerminal.colors.h"
    "src/ImGuiTerminal.keys.h"
    "src/nonspacing.h"
    "src/wide.h"
    "src/WindowsErrors.h"
)
set(HEXE_TERMINAL_SOURCES ${HEXE_TERMINAL_SOURCES}
    "src/AutoHandle.cpp"
    "src/boxdraw.cpp"
    "src/Process.cpp"
    "src/Process.win32.cpp"
    "src/PseudoTerminal.cpp"
    "src/PseudoTerminal.win32.cpp"
    "src/TerminalDisplay.cpp"
    "src/TerminalEmulator.cpp"
)

add_library(HexeTerminal ${HEXE_TERMINAL_HEADERS} ${HEXE_TERMINAL_SOURCES})
target_include_directories(HexeTerminal PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(IMGUI_TERMINAL_HEADERS ${IMGUI_TERMINAL_HEADERS}
    "include/Hexe/Terminal/ImGuiTerminal.h"
)
set(IMGUI_TERMINAL_SOURCES ${IMGUI_TERMINAL_SOURCES}
    "src/ImGuiTerminal.cpp"
    "src/ImGuiTerminal.keys.cpp"
)

add_library(ImGuiTerminal ${IMGUI_TERMINAL_HEADERS} ${IMGUI_TERMINAL_SOURCES})
target_link_libraries(ImGuiTerminal PUBLIC imgui HexeTerminal SDL2::SDL2)
target_compile_definitions(ImGuiTerminal PUBLIC HEXE_USING_SDL)

add_subdirectory(examples)