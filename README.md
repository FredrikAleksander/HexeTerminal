HexeTerminal (WIP)
==================================

HexeTerminal is a C++ library for integrating terminal emulator functionality into a application. It is based on the suckless st terminal emulator,
but the global C variables have become C++ class member variables. This allows running multiple terminal emulators.

HexeTerminal does not do any drawing (besides the internal buffer) and must be provided with a display that must be implemented by the user. 
If user input is desired, that must also be provided by the user. A reference implementation that uses ImGui and some examples utilizing it are provided

Building
==================================

In most cases you can just copy the source files you need into your project and build it as part of your project, or use git submodules.
To build the examples, CMake, SDL2 and GLEW is required


Windows
==================================

HexeTerminal uses ConPTY on Windows, which is a recent Windows 10 feature which is similar to Unix pseudo terminals. Recent Windows 10 builds are recommended for the best experience, as there are some buggy versions. I plan on adding a example that demonstrates how to build a more recent ConPTY library and Console host from the Windows Terminal sources as a way for an application that use its own version that is bleeding edge

ImGui reference implementation
==================================
A reference implementation that integrates with ImGui is provided. It is currently missing some input code for ImGui backends that do not use SDL

Examples
==================================
