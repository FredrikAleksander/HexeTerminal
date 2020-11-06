// dear imgui: wrapper to use FreeType (instead of stb_truetype)
// Get latest version at https://github.com/ocornut/imgui/tree/master/misc/freetype
// Original code by @Vuhdo (Aleksei Skriabin), maintained by @ocornut

#pragma once

#include "imgui.h" // IMGUI_API, ImFontAtlas

namespace ImGuiFreeTypeEx
{
    // This is a modification to the original ImGuiFreeType which has been extended to allow embedding
    // color emoji. The way this works is a bit hacky, and it is very opinionated on what emoji are embedded
    // and what formats are supported (depends also on FreeType version and compiled in features).
    // This was made as a very hacky proof of concept for using color emoji in the HexeTerminal ImGui widget.
    // Black and white emoji does not need anything special to work. Just merge them into your main font and it
    // should more or less work.

    // Because this was made for a terminal emulator, which needs monospace fonts, emojis are really messy.
    // Alot of applications (running in a terminal) are not properly aware of them, and cannot calculate their widths properly,
    // and rendering issues may appear. So even if the terminal emulator does everything perfectly, unless
    // every application that is run inside the terminal is also on the same page, stuff wont work.
    // This isnt unique to emojis, but emojis breaks stuff easier. This has to do with how some old characters
    // (or a newly assigned codepoint in a previously assigned unicode block) are now emojis, and their width
    // might have gone from half to full-width. In many cases the unicode spec does not specify the width
    // of emoji (leaving it ambigious), so there is no clear cut answer. In non monospace applications this doesnt
    // matter, but for monospace applications it matters, as it may cause horrible looking misalignment issues.
    // So this loader will only load emojis with defined widths, and they are all full-width. It will not be complete,
    // but hopefully compatible with most used emojis, and most used applications (I'm testing against Vim/Neovim and Emacs)
    
    // Another issue with color emojis, are how pretty much every big company out there, has made their own extension to
    // TrueType/OpenType in order to represent them. There are bitmap types, there are outline types which has multiple layers each with their own color,
    // and there is another variant that embeds SVG files into the font. If the SVG type of font is going to be used,
    // a SVG rendering library must also be available for rendering the SVG files into glyph bitmaps.


    // Hinting greatly impacts visuals (and glyph sizes).
    // When disabled, FreeType generates blurrier glyphs, more or less matches the stb's output.
    // The Default hinting mode usually looks good, but may distort glyphs in an unusual way.
    // The Light hinting mode generates fuzzier glyphs but better matches Microsoft's rasterizer.

    // You can set those flags on a per font basis in ImFontConfig::RasterizerFlags.
    // Use the 'extra_flags' parameter of BuildFontAtlas() to force a flag on all your fonts.
    enum RasterizerFlags
    {
        // By default, hinting is enabled and the font's native hinter is preferred over the auto-hinter.
        NoHinting = 1 << 0,     // Disable hinting. This generally generates 'blurrier' bitmap glyphs when the glyph are rendered in any of the anti-aliased modes.
        NoAutoHint = 1 << 1,    // Disable auto-hinter.
        ForceAutoHint = 1 << 2, // Indicates that the auto-hinter is preferred over the font's native hinter.
        LightHinting = 1 << 3,  // A lighter hinting algorithm for gray-level modes. Many generated glyphs are fuzzier but better resemble their original shape. This is achieved by snapping glyphs to the pixel grid only vertically (Y-axis), as is done by Microsoft's ClearType and Adobe's proprietary font renderer. This preserves inter-glyph spacing in horizontal text.
        MonoHinting = 1 << 4,   // Strong hinting algorithm that should only be used for monochrome output.
        Bold = 1 << 5,          // Styling: Should we artificially embolden the font?
        Oblique = 1 << 6,       // Styling: Should we slant the font, emulating italic style?
        Monochrome = 1 << 7,    // Disable anti-aliasing. Combine this with MonoHinting for best results!
        EmbedEmoji = 1 << 8,    // Embed color emojis into this font (for bw emoji, just merge fonts, only color emoji needs special care)
    };

    IMGUI_API bool BuildFontAtlas(ImFontAtlas *atlas, unsigned int extra_flags = 0, const ImVector<unsigned char>& color_emoji_font = {});

    // By default ImGuiFreeType will use IM_ALLOC()/IM_FREE().
    // However, as FreeType does lots of allocations we provide a way for the user to redirect it to a separate memory heap if desired:
    IMGUI_API void SetAllocatorFunctions(void *(*alloc_func)(size_t sz, void *user_data), void (*free_func)(void *ptr, void *user_data), void *user_data = NULL);
} // namespace ImGuiFreeType
