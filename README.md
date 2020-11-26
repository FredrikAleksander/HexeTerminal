# HexeTerminal (WIP)

HexeTerminal is a C++ library for integrating terminal emulator functionality into a application. It is based on the suckless st terminal emulator,
but the global C variables have become C++ class member variables. This allows running multiple terminal emulators.

HexeTerminal does not do any drawing (besides the internal buffer) and must be provided with a display that must be implemented by the user.
If user input is desired, that must also be provided by the user. A reference implementation that uses ImGui and some examples utilizing it are provided

# Features

- Truecolor support
- Wide character support
- Text Attributes (bold, italic, underline, strikethrough, reverse, invisible etc)
- Uses C++ interfaces which may be replaced by user implementations to manage processes and pipes/pseudoterminals
- Based on the suckless st terminal emulator, is compatable with the same TERM values
- Color Emoji support
- Fully capable of running Tmux, VIM, Emacs and your favorite terminal based roguelike

# Building

In most cases you can just copy the source files you need into your project and build it as part of your project, or use git submodules.
To build the examples, CMake, SDL2 and GLEW is required

# Extensibility

It is possible to use user provided implementations of IProcess, and IPseudoTerminal to implement a terminal that interacts with user code instead of spawning a new process.
This could be used for interacting with user code that needs a terminal, or it could be used to implement a SSH terminal by using a SSH session to manage processes and pseudoterminals on a remote host.

There is a interface IProcessFactory that is optional, but highly recommended for use in spawning processes and creating an associated pseudoterminal. When
all your code uses this interface to manage processes and pseudoterminals, adding support for different types of processes and pseudoterminals become much easier.

# Windows

HexeTerminal uses ConPTY on Windows, which is a recent Windows 10 feature which is similar to Unix pseudo terminals. Recent Windows 10 builds are recommended for the best experience, as there are some buggy versions. I plan on adding a example that demonstrates how to build a more recent ConPTY library and Console host from the Windows Terminal sources as a way for an application that use its own version that is bleeding edge

# ImGui reference implementation

A reference implementation that integrates with ImGui is provided. It is currently missing some input code for ImGui backends that do not use SDL.

# Examples

The examples directory contains some examples of how to use the library with the ImGui reference implementation.

## terminal

This example demonstrates a simple pure terminal emulator. It will fill the window with the terminal, and it will set the global window title from the terminal title. When the shell closes, the application closes aswell.

It supports loading a full set of fonts (regular, bold, italic, bold italic), which is currently hardcoded to the JetBrains Mono nerd font files. These are not provided in the repo and must be downloaded and copied to the same folder as the terminal emulator executable. It will fallback to the builtin ImGui default font if it cannot find the JetBrains Mono font files.

It will also attempt to load color emojis from a file NotoColorEmoji.ttf, if that file is found in the same folder as the executable. With a recent enough freetype build, it should support all common emoji font formats, except SVGinOTF (used by twitter)


# Windows

Here is an incoherent rant from a madman about ptys on Windows.

Windows only recently added support for pseudoterminals, and most Windows builds have somewhat buggy implementations. The good news is that it will eventually resolve itself,
as more recent builds of Windows are pushed out, but until then, alot of Windows users will suffer a degraded experience due to this. Some of these bugs are really weird,
and are often difficult to track down. There is however a few solutions to these problems, but they are out of scope of this project, and purely Windows specific, so I do not
want to make it part of the library, but I can explain how one would work around all these problems, and distribute a bleeding edge version of the pseudoterminal API.
The new Windows Terminal source also contains the code for a userspace library that implements the ConPTY API, and which may be built as a static library (MIT licensed), or as a DLL
and bundled with the application. It also contains code for the console host (aka conhost.exe), which may built as OpenConsole.exe and when located in the same folder as the application (if linking to a static ConPTY), or the folder of the ConPTY dll, will be used as the console host invoked by ConPTY. Most of the issues I've encountered is in the console host, and it is even possible to use a build you've compiled yourself and replace your system conhost.exe with it, and it will work (NOT RECOMMENDED, do not do this), infact I run with that change (again, do not do this, just because I'm a moron doesn't mean you have to be). Some of the issues I've experienced are stuff like certain attributes not working (italic, strikethrough, underline), certain characters, especially emojis etc completely messes up the display. So in the end if you plan on distributing your application on Windows, you may consider building
and distributing a custom built ConPTY dll and OpenConsole.exe console host with your application, to ensure a consistent user experience. It seems this is the route that Windows Terminal is going. The Windows Terminal repository at Github is highly recommended for more information on how this stuff works on Windows. And it is all MIT licensed (even the console host, which is part of Windows itself)