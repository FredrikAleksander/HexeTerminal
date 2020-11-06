// The MIT License (MIT)

// Copyright (c) 2020 Fredrik A. Kristiansen

//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.

#include "ImGuiTerminal.keys.h"

#define ImGuiKeyModFlags_AltGr ImGuiKeyModFlags_Ctrl | ImGuiKeyModFlags_Alt

ImGuiKeyMap::ImGuiKeyMap(const Key keys[], size_t keysLen, const Key platformKeys[], size_t platformKeysLen, const Shortcut shortcuts[], size_t shortcutsLen)
{
    for (size_t i = 0; i < keysLen; i++)
    {
        auto &e = m_keyMap[keys[i].keysym];
        e.push_back({keys[i].mask, keys[i].string, keys[i].appkey, keys[i].appcursor});
    }

    for (size_t i = 0; i < platformKeysLen; i++)
    {
        auto &e = m_platformKeyMap[platformKeys[i].keysym];
        e.push_back({platformKeys[i].mask, platformKeys[i].string, platformKeys[i].appkey, platformKeys[i].appcursor});
    }

    for (size_t i = 0; i < shortcutsLen; i++)
    {
        auto &e = m_shortcuts[shortcuts[i].keysym];
        e.push_back({shortcuts[i].mask, shortcuts[i].action, shortcuts[i].appkey, shortcuts[i].appcursor});
    }
}

using ShortcutAction = Hexe::Terminal::ShortcutAction;

static Shortcut shortcuts[] = {
    {ImGuiKey_Insert, ImGuiKeyModFlags_Shift, ShortcutAction::PASTE, 0, 0}};

static Key keys[] = {
    /* keysym           mask            string      appkey appcursor */
    {ImGuiKey_KeyPadEnter, ImGuiKeyModFlags_Any, "\033OM", +2, 0},
    {ImGuiKey_KeyPadEnter, ImGuiKeyModFlags_Any, "\r", -1, 0},

    {ImGuiKey_UpArrow, ImGuiKeyModFlags_Shift, "\033[1;2A", 0, 0},
    {ImGuiKey_UpArrow, ImGuiKeyModFlags_Alt, "\033[1;3A", 0, 0},
    {ImGuiKey_UpArrow, ImGuiKeyModFlags_Shift | ImGuiKeyModFlags_Alt, "\033[1;4A", 0, 0},
    {ImGuiKey_UpArrow, ImGuiKeyModFlags_Ctrl, "\033[1;5A", 0, 0},
    {ImGuiKey_UpArrow, ImGuiKeyModFlags_Shift | ImGuiKeyModFlags_Ctrl, "\033[1;6A", 0, 0},
    {ImGuiKey_UpArrow, ImGuiKeyModFlags_Ctrl | ImGuiKeyModFlags_Alt, "\033[1;7A", 0, 0},
    {ImGuiKey_UpArrow, ImGuiKeyModFlags_Shift | ImGuiKeyModFlags_Ctrl | ImGuiKeyModFlags_Alt, "\033[1;8A", 0, 0},
    {ImGuiKey_UpArrow, ImGuiKeyModFlags_Any, "\033[A", 0, -1},
    {ImGuiKey_UpArrow, ImGuiKeyModFlags_Any, "\033OA", 0, +1},
    {ImGuiKey_DownArrow, ImGuiKeyModFlags_Shift, "\033[1;2B", 0, 0},
    {ImGuiKey_DownArrow, ImGuiKeyModFlags_Alt, "\033[1;3B", 0, 0},
    {ImGuiKey_DownArrow, ImGuiKeyModFlags_Shift | ImGuiKeyModFlags_Alt, "\033[1;4B", 0, 0},
    {ImGuiKey_DownArrow, ImGuiKeyModFlags_Ctrl, "\033[1;5B", 0, 0},
    {ImGuiKey_DownArrow, ImGuiKeyModFlags_Shift | ImGuiKeyModFlags_Ctrl, "\033[1;6B", 0, 0},
    {ImGuiKey_DownArrow, ImGuiKeyModFlags_Ctrl | ImGuiKeyModFlags_Alt, "\033[1;7B", 0, 0},
    {ImGuiKey_DownArrow, ImGuiKeyModFlags_Shift | ImGuiKeyModFlags_Ctrl | ImGuiKeyModFlags_Alt, "\033[1;8B", 0, 0},
    {ImGuiKey_DownArrow, ImGuiKeyModFlags_Any, "\033[B", 0, -1},
    {ImGuiKey_DownArrow, ImGuiKeyModFlags_Any, "\033OB", 0, +1},
    {ImGuiKey_LeftArrow, ImGuiKeyModFlags_Shift, "\033[1;2D", 0, 0},
    {ImGuiKey_LeftArrow, ImGuiKeyModFlags_Alt, "\033[1;3D", 0, 0},
    {ImGuiKey_LeftArrow, ImGuiKeyModFlags_Shift | ImGuiKeyModFlags_Alt, "\033[1;4D", 0, 0},
    {ImGuiKey_LeftArrow, ImGuiKeyModFlags_Ctrl, "\033[1;5D", 0, 0},
    {ImGuiKey_LeftArrow, ImGuiKeyModFlags_Shift | ImGuiKeyModFlags_Ctrl, "\033[1;6D", 0, 0},
    {ImGuiKey_LeftArrow, ImGuiKeyModFlags_Ctrl | ImGuiKeyModFlags_Alt, "\033[1;7D", 0, 0},
    {ImGuiKey_LeftArrow, ImGuiKeyModFlags_Shift | ImGuiKeyModFlags_Ctrl | ImGuiKeyModFlags_Alt, "\033[1;8D", 0, 0},
    {ImGuiKey_LeftArrow, ImGuiKeyModFlags_Any, "\033[D", 0, -1},
    {ImGuiKey_LeftArrow, ImGuiKeyModFlags_Any, "\033OD", 0, +1},
    {ImGuiKey_RightArrow, ImGuiKeyModFlags_Shift, "\033[1;2C", 0, 0},
    {ImGuiKey_RightArrow, ImGuiKeyModFlags_Alt, "\033[1;3C", 0, 0},
    {ImGuiKey_RightArrow, ImGuiKeyModFlags_Shift | ImGuiKeyModFlags_Alt, "\033[1;4C", 0, 0},
    {ImGuiKey_RightArrow, ImGuiKeyModFlags_Ctrl, "\033[1;5C", 0, 0},
    {ImGuiKey_RightArrow, ImGuiKeyModFlags_Shift | ImGuiKeyModFlags_Ctrl, "\033[1;6C", 0, 0},
    {ImGuiKey_RightArrow, ImGuiKeyModFlags_Ctrl | ImGuiKeyModFlags_Alt, "\033[1;7C", 0, 0},
    {ImGuiKey_RightArrow, ImGuiKeyModFlags_Shift | ImGuiKeyModFlags_Ctrl | ImGuiKeyModFlags_Alt, "\033[1;8C", 0, 0},
    {ImGuiKey_RightArrow, ImGuiKeyModFlags_Any, "\033[C", 0, -1},
    {ImGuiKey_RightArrow, ImGuiKeyModFlags_Any, "\033OC", 0, +1},
    {ImGuiKey_Tab, ImGuiKeyModFlags_Shift, "\033[Z", 0, 0},
    {ImGuiKey_Tab, ImGuiKeyModFlags_Any, "\t", 0, 0},
    {ImGuiKey_Enter, ImGuiKeyModFlags_Alt, "\033\r", 0, 0},
    {ImGuiKey_Enter, ImGuiKeyModFlags_Any, "\r", 0, 0},
    {ImGuiKey_Insert, ImGuiKeyModFlags_Shift, "\033[4l", -1, 0},
    {ImGuiKey_Insert, ImGuiKeyModFlags_Shift, "\033[2;2~", +1, 0},
    {ImGuiKey_Insert, ImGuiKeyModFlags_Ctrl, "\033[L", -1, 0},
    {ImGuiKey_Insert, ImGuiKeyModFlags_Ctrl, "\033[2;5~", +1, 0},
    {ImGuiKey_Insert, ImGuiKeyModFlags_Any, "\033[4h", -1, 0},
    {ImGuiKey_Insert, ImGuiKeyModFlags_Any, "\033[2~", +1, 0},
    {ImGuiKey_Delete, ImGuiKeyModFlags_Ctrl, "\033[M", -1, 0},
    {ImGuiKey_Delete, ImGuiKeyModFlags_Ctrl, "\033[3;5~", +1, 0},
    {ImGuiKey_Delete, ImGuiKeyModFlags_Shift, "\033[2K", -1, 0},
    {ImGuiKey_Delete, ImGuiKeyModFlags_Shift, "\033[3;2~", +1, 0},
    {ImGuiKey_Delete, ImGuiKeyModFlags_Any, "\033[P", -1, 0},
    {ImGuiKey_Delete, ImGuiKeyModFlags_Any, "\033[3~", +1, 0},
    {ImGuiKey_Backspace, ImGuiKeyModFlags_None, "\177", 0, 0},
    {ImGuiKey_Backspace, ImGuiKeyModFlags_Alt, "\033\177", 0, 0},
    {ImGuiKey_Home, ImGuiKeyModFlags_Shift, "\033[2J", 0, -1},
    {ImGuiKey_Home, ImGuiKeyModFlags_Shift, "\033[1;2H", 0, +1},
    {ImGuiKey_Home, ImGuiKeyModFlags_Any, "\033[H", 0, -1},
    {ImGuiKey_Home, ImGuiKeyModFlags_Any, "\033[1~", 0, +1},
    {ImGuiKey_End, ImGuiKeyModFlags_Ctrl, "\033[J", -1, 0},
    {ImGuiKey_End, ImGuiKeyModFlags_Ctrl, "\033[1;5F", +1, 0},
    {ImGuiKey_End, ImGuiKeyModFlags_Shift, "\033[K", -1, 0},
    {ImGuiKey_End, ImGuiKeyModFlags_Shift, "\033[1;2F", +1, 0},
    {ImGuiKey_End, ImGuiKeyModFlags_Any, "\033[4~", 0, 0},
    {ImGuiKey_Escape, ImGuiKeyModFlags_Any, "\033", 0, 0},

    {ImGuiKey_PageUp, ImGuiKeyModFlags_None, "\033[5~", 0, 0},
    {ImGuiKey_PageDown, ImGuiKeyModFlags_None, "\033[6~", 0, 0},
};
static Key platformKeys[] = {
#if defined(HEXE_USING_SDL)
    {SDL_SCANCODE_PRIOR, ImGuiKeyModFlags_Ctrl, "\033[5;5~", 0, 0},

    {SDL_SCANCODE_PRIOR, ImGuiKeyModFlags_Ctrl, "\033[5;5~", 0, 0},
    {SDL_SCANCODE_PRIOR, ImGuiKeyModFlags_Shift, "\033[5;2~", 0, 0},
    {SDL_SCANCODE_PRIOR, ImGuiKeyModFlags_Any, "\033[5~", 0, 0},

    {SDL_SCANCODE_KP_BACKSPACE, ImGuiKeyModFlags_Ctrl, "\033[M", -1, 0},
    {SDL_SCANCODE_KP_BACKSPACE, ImGuiKeyModFlags_Ctrl, "\033[3;5~", +1, 0},
    {SDL_SCANCODE_KP_BACKSPACE, ImGuiKeyModFlags_Shift, "\033[2K", -1, 0},
    {SDL_SCANCODE_KP_BACKSPACE, ImGuiKeyModFlags_Shift, "\033[3;2~", +1, 0},
    {SDL_SCANCODE_KP_BACKSPACE, ImGuiKeyModFlags_Any, "\033[P", -1, 0},
    {SDL_SCANCODE_KP_BACKSPACE, ImGuiKeyModFlags_Any, "\033[3~", +1, 0},

    {SDL_SCANCODE_KP_MINUS, ImGuiKeyModFlags_Any, "\033Om", +2, 0},
    {SDL_SCANCODE_KP_DECIMAL, ImGuiKeyModFlags_Any, "\033On", +2, 0},
    {SDL_SCANCODE_KP_DIVIDE, ImGuiKeyModFlags_Any, "\033Oo", +2, 0},
    {SDL_SCANCODE_KP_MULTIPLY, ImGuiKeyModFlags_Any, "\033Oj", +2, 0},
    {SDL_SCANCODE_KP_PLUS, ImGuiKeyModFlags_Any, "\033Ok", +2, 0},

    {SDL_SCANCODE_F1, ImGuiKeyModFlags_None, "\033OP", 0, 0},
    {SDL_SCANCODE_F1, /* F13 */ ImGuiKeyModFlags_Shift, "\033[1;2P", 0, 0},
    {SDL_SCANCODE_F1, /* F25 */ ImGuiKeyModFlags_Ctrl, "\033[1;5P", 0, 0},
    {SDL_SCANCODE_F1, /* F37 */ ImGuiKeyModFlags_Super, "\033[1;6P", 0, 0},
    {SDL_SCANCODE_F1, /* F49 */ ImGuiKeyModFlags_Alt, "\033[1;3P", 0, 0},
    {SDL_SCANCODE_F1, /* F61 */ ImGuiKeyModFlags_AltGr, "\033[1;4P", 0, 0},
    {SDL_SCANCODE_F2, ImGuiKeyModFlags_None, "\033OQ", 0, 0},
    {SDL_SCANCODE_F2, /* F14 */ ImGuiKeyModFlags_Shift, "\033[1;2Q", 0, 0},
    {SDL_SCANCODE_F2, /* F26 */ ImGuiKeyModFlags_Ctrl, "\033[1;5Q", 0, 0},
    {SDL_SCANCODE_F2, /* F38 */ ImGuiKeyModFlags_Super, "\033[1;6Q", 0, 0},
    {SDL_SCANCODE_F2, /* F50 */ ImGuiKeyModFlags_Alt, "\033[1;3Q", 0, 0},
    {SDL_SCANCODE_F2, /* F62 */ ImGuiKeyModFlags_AltGr, "\033[1;4Q", 0, 0},
    {SDL_SCANCODE_F3, ImGuiKeyModFlags_None, "\033OR", 0, 0},
    {SDL_SCANCODE_F3, /* F15 */ ImGuiKeyModFlags_Shift, "\033[1;2R", 0, 0},
    {SDL_SCANCODE_F3, /* F27 */ ImGuiKeyModFlags_Ctrl, "\033[1;5R", 0, 0},
    {SDL_SCANCODE_F3, /* F39 */ ImGuiKeyModFlags_Super, "\033[1;6R", 0, 0},
    {SDL_SCANCODE_F3, /* F51 */ ImGuiKeyModFlags_Alt, "\033[1;3R", 0, 0},
    {SDL_SCANCODE_F3, /* F63 */ ImGuiKeyModFlags_AltGr, "\033[1;4R", 0, 0},
    {SDL_SCANCODE_F4, ImGuiKeyModFlags_None, "\033OS", 0, 0},
    {SDL_SCANCODE_F4, /* F16 */ ImGuiKeyModFlags_Shift, "\033[1;2S", 0, 0},
    {SDL_SCANCODE_F4, /* F28 */ ImGuiKeyModFlags_Ctrl, "\033[1;5S", 0, 0},
    {SDL_SCANCODE_F4, /* F40 */ ImGuiKeyModFlags_Super, "\033[1;6S", 0, 0},
    {SDL_SCANCODE_F4, /* F52 */ ImGuiKeyModFlags_Alt, "\033[1;3S", 0, 0},
    {SDL_SCANCODE_F5, ImGuiKeyModFlags_None, "\033[15~", 0, 0},
    {SDL_SCANCODE_F5, /* F17 */ ImGuiKeyModFlags_Shift, "\033[15;2~", 0, 0},
    {SDL_SCANCODE_F5, /* F29 */ ImGuiKeyModFlags_Ctrl, "\033[15;5~", 0, 0},
    {SDL_SCANCODE_F5, /* F41 */ ImGuiKeyModFlags_Super, "\033[15;6~", 0, 0},
    {SDL_SCANCODE_F5, /* F53 */ ImGuiKeyModFlags_Alt, "\033[15;3~", 0, 0},
    {SDL_SCANCODE_F6, ImGuiKeyModFlags_None, "\033[17~", 0, 0},
    {SDL_SCANCODE_F6, /* F18 */ ImGuiKeyModFlags_Shift, "\033[17;2~", 0, 0},
    {SDL_SCANCODE_F6, /* F30 */ ImGuiKeyModFlags_Ctrl, "\033[17;5~", 0, 0},
    {SDL_SCANCODE_F6, /* F42 */ ImGuiKeyModFlags_Super, "\033[17;6~", 0, 0},
    {SDL_SCANCODE_F6, /* F54 */ ImGuiKeyModFlags_Alt, "\033[17;3~", 0, 0},
    {SDL_SCANCODE_F7, ImGuiKeyModFlags_None, "\033[18~", 0, 0},
    {SDL_SCANCODE_F7, /* F19 */ ImGuiKeyModFlags_Shift, "\033[18;2~", 0, 0},
    {SDL_SCANCODE_F7, /* F31 */ ImGuiKeyModFlags_Ctrl, "\033[18;5~", 0, 0},
    {SDL_SCANCODE_F7, /* F43 */ ImGuiKeyModFlags_Super, "\033[18;6~", 0, 0},
    {SDL_SCANCODE_F7, /* F55 */ ImGuiKeyModFlags_Alt, "\033[18;3~", 0, 0},
    {SDL_SCANCODE_F8, ImGuiKeyModFlags_None, "\033[19~", 0, 0},
    {SDL_SCANCODE_F8, /* F20 */ ImGuiKeyModFlags_Shift, "\033[19;2~", 0, 0},
    {SDL_SCANCODE_F8, /* F32 */ ImGuiKeyModFlags_Ctrl, "\033[19;5~", 0, 0},
    {SDL_SCANCODE_F8, /* F44 */ ImGuiKeyModFlags_Super, "\033[19;6~", 0, 0},
    {SDL_SCANCODE_F8, /* F56 */ ImGuiKeyModFlags_Alt, "\033[19;3~", 0, 0},
    {SDL_SCANCODE_F9, ImGuiKeyModFlags_None, "\033[20~", 0, 0},
    {SDL_SCANCODE_F9, /* F21 */ ImGuiKeyModFlags_Shift, "\033[20;2~", 0, 0},
    {SDL_SCANCODE_F9, /* F33 */ ImGuiKeyModFlags_Ctrl, "\033[20;5~", 0, 0},
    {SDL_SCANCODE_F9, /* F45 */ ImGuiKeyModFlags_Super, "\033[20;6~", 0, 0},
    {SDL_SCANCODE_F9, /* F57 */ ImGuiKeyModFlags_Alt, "\033[20;3~", 0, 0},
    {SDL_SCANCODE_F10, ImGuiKeyModFlags_None, "\033[21~", 0, 0},
    {SDL_SCANCODE_F10, /* F22 */ ImGuiKeyModFlags_Shift, "\033[21;2~", 0, 0},
    {SDL_SCANCODE_F10, /* F34 */ ImGuiKeyModFlags_Ctrl, "\033[21;5~", 0, 0},
    {SDL_SCANCODE_F10, /* F46 */ ImGuiKeyModFlags_Super, "\033[21;6~", 0, 0},
    {SDL_SCANCODE_F10, /* F58 */ ImGuiKeyModFlags_Alt, "\033[21;3~", 0, 0},
    {SDL_SCANCODE_F11, ImGuiKeyModFlags_None, "\033[23~", 0, 0},
    {SDL_SCANCODE_F11, /* F23 */ ImGuiKeyModFlags_Shift, "\033[23;2~", 0, 0},
    {SDL_SCANCODE_F11, /* F35 */ ImGuiKeyModFlags_Ctrl, "\033[23;5~", 0, 0},
    {SDL_SCANCODE_F11, /* F47 */ ImGuiKeyModFlags_Super, "\033[23;6~", 0, 0},
    {SDL_SCANCODE_F11, /* F59 */ ImGuiKeyModFlags_Alt, "\033[23;3~", 0, 0},
    {SDL_SCANCODE_F12, ImGuiKeyModFlags_None, "\033[24~", 0, 0},
    {SDL_SCANCODE_F12, /* F24 */ ImGuiKeyModFlags_Shift, "\033[24;2~", 0, 0},
    {SDL_SCANCODE_F12, /* F36 */ ImGuiKeyModFlags_Ctrl, "\033[24;5~", 0, 0},
    {SDL_SCANCODE_F12, /* F48 */ ImGuiKeyModFlags_Super, "\033[24;6~", 0, 0},
    {SDL_SCANCODE_F12, /* F60 */ ImGuiKeyModFlags_Alt, "\033[24;3~", 0, 0},
    {SDL_SCANCODE_F13, ImGuiKeyModFlags_None, "\033[1;2P", 0, 0},
    {SDL_SCANCODE_F14, ImGuiKeyModFlags_None, "\033[1;2Q", 0, 0},
    {SDL_SCANCODE_F15, ImGuiKeyModFlags_None, "\033[1;2R", 0, 0},
    {SDL_SCANCODE_F16, ImGuiKeyModFlags_None, "\033[1;2S", 0, 0},
    {SDL_SCANCODE_F17, ImGuiKeyModFlags_None, "\033[15;2~", 0, 0},
    {SDL_SCANCODE_F18, ImGuiKeyModFlags_None, "\033[17;2~", 0, 0},
    {SDL_SCANCODE_F19, ImGuiKeyModFlags_None, "\033[18;2~", 0, 0},
    {SDL_SCANCODE_F20, ImGuiKeyModFlags_None, "\033[19;2~", 0, 0},
    {SDL_SCANCODE_F21, ImGuiKeyModFlags_None, "\033[20;2~", 0, 0},
    {SDL_SCANCODE_F22, ImGuiKeyModFlags_None, "\033[21;2~", 0, 0},
    {SDL_SCANCODE_F23, ImGuiKeyModFlags_None, "\033[23;2~", 0, 0},
    {SDL_SCANCODE_F24, ImGuiKeyModFlags_None, "\033[24;2~", 0, 0},

    {SDL_SCANCODE_KP_0, ImGuiKeyModFlags_Any, "\033Op", +2, 0},
    {SDL_SCANCODE_KP_1, ImGuiKeyModFlags_Any, "\033Oq", +2, 0},
    {SDL_SCANCODE_KP_2, ImGuiKeyModFlags_Any, "\033Or", +2, 0},
    {SDL_SCANCODE_KP_3, ImGuiKeyModFlags_Any, "\033Os", +2, 0},
    {SDL_SCANCODE_KP_4, ImGuiKeyModFlags_Any, "\033Ot", +2, 0},
    {SDL_SCANCODE_KP_5, ImGuiKeyModFlags_Any, "\033Ou", +2, 0},
    {SDL_SCANCODE_KP_6, ImGuiKeyModFlags_Any, "\033Ov", +2, 0},
    {SDL_SCANCODE_KP_7, ImGuiKeyModFlags_Any, "\033Ow", +2, 0},
    {SDL_SCANCODE_KP_8, ImGuiKeyModFlags_Any, "\033Ox", +2, 0},
    {SDL_SCANCODE_KP_9, ImGuiKeyModFlags_Any, "\033Oy", +2, 0}
#endif
};

ImGuiKeyMap ImGuiTerminalKeyMap{keys, (sizeof(keys) / sizeof(keys[0])), platformKeys, (sizeof(platformKeys) / sizeof(platformKeys[0])), shortcuts, (sizeof(shortcuts) / sizeof(shortcuts[0]))};