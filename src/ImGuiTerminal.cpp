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

#include "Hexe/Terminal/ImGuiTerminal.h"
#include "Hexe/Terminal/Boxdraw.h"
#include "Hexe/System/Process.h"
#include "Hexe/System/ProcessFactory.h"
//#include "Macros.h"
#include "ImGuiTerminal.colors.h"
#include "ImGuiTerminal.keys.h"
#include "imgui_internal.h"
#include <cmath>

#undef min
#undef max

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define LEN(a) (sizeof(a) / sizeof(a)[0])
#define BETWEEN(x, a, b) ((a) <= (x) && (x) <= (b))
#define IS_SET(flag) ((m_mode & (flag)) != 0)
#define DIV(n, d) (((n) + (d) / 2.0f) / (d))
#define DIVI(n, d) (((n) + (d) / 2) / (d))

using namespace Hexe::Terminal;

#ifdef _MSC_VER
#pragma warning(push)

#pragma warning(disable : 4996)
#endif

inline static void drawrect(ImU32 col, float x, float y, float w, float h, ImDrawVert *&vtx_write, ImDrawIdx *&idx_write, unsigned int &vtx_current_idx, const ImVec2 &uv)
{
    ImVec2 a(x, y), c(x + w, y + h);
    ImVec2 b(c.x, a.y), d(a.x, c.y);
    idx_write[0] = vtx_current_idx;
    idx_write[1] = (ImDrawIdx)(vtx_current_idx + 1);
    idx_write[2] = (ImDrawIdx)(vtx_current_idx + 2);
    idx_write[3] = vtx_current_idx;
    idx_write[4] = (ImDrawIdx)(vtx_current_idx + 2);
    idx_write[5] = (ImDrawIdx)(vtx_current_idx + 3);
    vtx_write[0].pos = a;
    vtx_write[0].uv = uv;
    vtx_write[0].col = col;
    vtx_write[1].pos = b;
    vtx_write[1].uv = uv;
    vtx_write[1].col = col;
    vtx_write[2].pos = c;
    vtx_write[2].uv = uv;
    vtx_write[2].col = col;
    vtx_write[3].pos = d;
    vtx_write[3].uv = uv;
    vtx_write[3].col = col;
    vtx_write += 4;
    vtx_current_idx += 4;
    idx_write += 6;
}

inline static void drawboxlines(float x, float y, float w, float h, ImU32 fg, ushort bd, ImDrawVert *&vtx_write, ImDrawIdx *&idx_write, unsigned int &vtx_current_idx, const ImVec2 &TexUvWhitePixel)
{
    /* s: stem thickness. width/8 roughly matches underscore thickness. */
    /* We draw bold as 1.5 * normal-stem and at least 1px thicker.      */
    /* doubles draw at least 3px, even when w or h < 3. bold needs 6px. */
    float mwh = MIN(w, h);
    float base_s = MAX(1.0f, DIV(mwh, 8.0f));
    int bold = (bd & BDB) && mwh >= 6.0f; /* possibly ignore boldness */
    float s = bold ? MAX(base_s + 1.0f, DIV(3.0f * base_s, 2.0f)) : base_s;
    float w2 = DIV(w - s, 2.0f), h2 = DIV(h - s, 2.0f);
    /* the s-by-s square (x + w2, y + h2, s, s) is the center texel.    */
    /* The base length (per direction till edge) includes this square.  */

    int light = bd & (LL | LU | LR | LD);
    int double_ = bd & (DL | DU | DR | DD);

    if (light)
    {
        /* d: additional (negative) length to not-draw the center   */
        /* texel - at arcs and avoid drawing inside (some) doubles  */
        int arc = bd & BDA;
        int multi_light = light & (light - 1);
        int multi_double = double_ & (double_ - 1);
        /* light crosses double only at DH+LV, DV+LH (ref. shapes)  */
        float d = arc || (multi_double && !multi_light) ? -s : 0.0f;

        if (bd & LL)
            drawrect(fg, x, y + h2, w2 + s + d, s, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
        if (bd & LU)
            drawrect(fg, x + w2, y, s, h2 + s + d, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
        if (bd & LR)
            drawrect(fg, x + w2 - d, y + h2, w - w2 + d, s, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
        if (bd & LD)
            drawrect(fg, x + w2, y + h2 - d, s, h - h2 + d, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
    }

    /* double lines - also align with light to form heavy when combined */
    if (double_)
    {
        /*
		* going clockwise, for each double-ray: p is additional length
		* to the single-ray nearer to the previous direction, and n to
		* the next. p and n adjust from the base length to lengths
		* which consider other doubles - shorter to avoid intersections
		* (p, n), or longer to draw the far-corner texel (n).
		*/
        int dl = bd & DL, du = bd & DU, dr = bd & DR, dd = bd & DD;
        if (dl)
        {
            float p = dd ? -s : 0.0f, n = du ? -s : dd ? s : 0.0f;
            drawrect(fg, x, y + h2 + s, w2 + s + p, s, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
            drawrect(fg, x, y + h2 - s, w2 + s + n, s, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
        }
        if (du)
        {
            float p = dl ? -s : 0.0f, n = dr ? -s : dl ? s : 0.0f;
            drawrect(fg, x + w2 - s, y, s, h2 + s + p, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
            drawrect(fg, x + w2 + s, y, s, h2 + s + n, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
        }
        if (dr)
        {
            float p = du ? -s : 0.0f, n = dd ? -s : du ? s : 0.0f;
            drawrect(fg, x + w2 - p, y + h2 - s, w - w2 + p, s, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
            drawrect(fg, x + w2 - n, y + h2 + s, w - w2 + n, s, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
        }
        if (dd)
        {
            float p = dr ? -s : 0.0f, n = dl ? -s : dr ? s : 0.0f;
            drawrect(fg, x + w2 + s, y + h2 - p, s, h - h2 + p, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
            drawrect(fg, x + w2 - s, y + h2 - n, s, h - h2 + n, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
        }
    }
}

inline static void drawbox(float x, float y, float w, float h, ImU32 fg, ImU32 bg, ushort bd, ImDrawVert *&vtx_write, ImDrawIdx *&idx_write, unsigned int &vtx_current_idx, const ImVec2 &TexUvWhitePixel)
{
    ushort cat = bd & ~(BDB | 0xff); /* mask out bold and data */
    if (bd & (BDL | BDA))
    {
        /* lines (light/double/heavy/arcs) */
        drawboxlines(x, y, w, h, fg, bd, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
    }
    else if (cat == BBD)
    {
        /* lower (8-X)/8 block */
        float d = DIV((bd & 0xFF) * h, 8.0f);
        drawrect(fg, x, y + d, w, h - d, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
    }
    else if (cat == BBU)
    {
        /* upper X/8 block */
        drawrect(fg, x, y, w, DIV((bd & 0xFF) * h, 8), vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
    }
    else if (cat == BBL)
    {
        /* left X/8 block */
        drawrect(fg, x, y, DIV((bd & 0xFF) * w, 8), h, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
    }
    else if (cat == BBR)
    {
        /* right (8-X)/8 block */
        float d = DIV((bd & 0xFF) * w, 8.0f);
        drawrect(fg, x + d, y, w - d, h, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
    }
    else if (cat == BBQ)
    {
        /* Quadrants */
        float w2 = DIV(w, 2.0f), h2 = DIV(h, 2.0f);
        if (bd & TL)
            drawrect(fg, x, y, w2, h2, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
        if (bd & TR)
            drawrect(fg, x + w2, y, w - w2, h2, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
        if (bd & BL)
            drawrect(fg, x, y + h2, w2, h - h2, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
        if (bd & BR)
            drawrect(fg, x + w2, y + h2, w - w2, h - h2, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
    }
    else if (bd & BBS)
    {
        /* Shades - data is 1/2/3 for 25%/50%/75% alpha, respectively */
        int d = (bd & 0xFF);

        ImU32 red = DIVI(((fg >> IM_COL32_R_SHIFT) & 0xFF) * d + ((bg >> IM_COL32_R_SHIFT) & 0xFF) * (4 - d), 4);
        ImU32 green = DIVI(((fg >> IM_COL32_G_SHIFT) & 0xFF) * d + ((bg >> IM_COL32_G_SHIFT) & 0xFF) * (4 - d), 4);
        ImU32 blue = DIVI(((fg >> IM_COL32_B_SHIFT) & 0xFF) * d + ((bg >> IM_COL32_B_SHIFT) & 0xFF) * (4 - d), 4);

        ImU32 drawcol = IM_COL32(red, green, blue, 0xFF);

        drawrect(drawcol, x, y, w, h, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
    }
    else if (cat == BRL)
    {
        /* braille, each data bit corresponds to one dot at 2x4 grid */
        float w1 = DIV(w, 2.0f);
        float h1 = DIV(h, 4.0f), h2 = DIV(h, 2.0f), h3 = DIV(3.0f * h, 4.0f);

        if (bd & 1)
            drawrect(fg, x, y, w1, h1, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
        if (bd & 2)
            drawrect(fg, x, y + h1, w1, h2 - h1, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
        if (bd & 4)
            drawrect(fg, x, y + h2, w1, h3 - h2, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
        if (bd & 8)
            drawrect(fg, x + w1, y, w - w1, h1, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
        if (bd & 16)
            drawrect(fg, x + w1, y + h1, w - w1, h2 - h1, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
        if (bd & 32)
            drawrect(fg, x + w1, y + h2, w - w1, h3 - h2, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
        if (bd & 64)
            drawrect(fg, x, y + h3, w1, h - h3, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
        if (bd & 128)
            drawrect(fg, x + w1, y + h3, w - w1, h - h3, vtx_write, idx_write, vtx_current_idx, TexUvWhitePixel);
    }
}

static inline ImU32 GetCol(unsigned int terminalColor, const ImVector<std::pair<ImU32, std::string>> &colors)
{
    if ((terminalColor & (1 << 24)) == 0)
    {
        return colors[terminalColor & 0xFF].first;
    }
    return IM_COL32((terminalColor >> 16) & 0xFF, (terminalColor >> 8) & 0xFF, terminalColor & 0xFF, (~((terminalColor >> 25) & 0xFF)) & 0xFF);
}

ImGuiTerminal::ImGuiTerminal(int columns, int rows, ImGuiTerminalConfig *config)
    : m_borderpx(1.0f), m_cursorthickness(2.0f), m_cursorx(0), m_cursory(0), m_cursorg({}), m_columns(columns), m_rows(rows), m_dirty(true), m_checkDirty(false), m_flags(0), m_useBoxDrawing(true), m_useColorEmoji(false), m_pasteNewlineFix(false), m_elapsedTime(0.0), m_lastBlink(0.0), m_defaultFont(nullptr), m_boldFont(nullptr), m_italicFont(nullptr), m_boldItalicFont(nullptr)
{
    Hexe::Terminal::Glyph defaultGlyph;
    defaultGlyph.mode = ATTR_INVISIBLE;
    auto defaultColor = std::make_pair<ImU32, std::string>(0U, "");
    m_cursorg = defaultGlyph;
    m_colors.resize(LEN(colornames), defaultColor);
    m_buffer.resize(m_columns * m_rows, defaultGlyph);
    ((int &)m_mode) |= MODE_FOCUSED;

    memset(&m_mouseState, 0, sizeof(m_mouseState));

    if (config != nullptr)
    {
        if (config->options & OPTION_COLOR_EMOJI)
            m_useColorEmoji = true;
        if (config->options & OPTION_NO_BOXDRAWING)
            m_useBoxDrawing = false;
        if (config->options & OPTION_PASTE_CRLF)
            m_pasteNewlineFix = true;
    }
}

void ImGuiTerminal::Update()
{
    m_terminal->Update();
}

void ImGuiTerminal::MouseReport(int x, int y, int button, int state, int type)
{
    if (button != 0)
    {
        return;
    }

    if (type == 1)
    {
        int tx = x;
        int ty = y;
        m_terminal->selextend(x, y, ImGui::GetIO().KeyMods == ImGuiKeyModFlags_Shift ? SEL_RECTANGULAR : SEL_REGULAR, 0);
    }
    else
    {
        if (state == 1)
        {
            m_mouseState.sx = x;
            m_mouseState.sy = y;
            m_terminal->selstart(x, y, 0);
        }
        else
        {
            auto selection = m_terminal->getsel();
            SetClipboard(selection);
            m_terminal->selclear();
        }
    }
}

void ImGuiTerminal::ProcessInput(int mouse_column, int mouse_row)
{
    auto &io = ImGui::GetIO();

    bool isHovered = ImGui::IsItemHovered();

    for (int i = 0; i < 5; i++)
    {
        if (m_mouseState.buttonDown[i] != io.MouseDown[i])
        {
            if (io.MouseDown[i])
            {
                if (io.MouseDownDuration[i] == 0.0f && isHovered)
                {
                    m_mouseState.buttonDown[i] = true;
                    MouseReport(mouse_column, mouse_row, i, 1, 0);
                }
            }
            else
            {
                m_mouseState.buttonDown[i] = false;
                MouseReport(mouse_column, mouse_row, i, 0, 0);
            }
        }
        else if (m_mouseState.buttonDown[i])
        {
            // Update motion
            MouseReport(mouse_column, mouse_row, i, 1, 1);
        }
    }

    auto keyModFlags = io.KeyMods;

    // Process regular text input (before we check for Return because using some IME will effectively send a Return?)
    // We ignore CTRL inputs, but need to allow ALT+CTRL as some keyboards (e.g. German) use AltGR (which _is_ Alt+Ctrl) to input certain characters.
    if (io.InputQueueCharacters.Size > 0)
    {
        if (keyModFlags != ImGuiKeyModFlags_Ctrl)
        {
            auto utf8InputLen = ImTextCountUtf8BytesFromStr(io.InputQueueCharacters.Data, io.InputQueueCharacters.Data + io.InputQueueCharacters.Size);
            std::vector<char> utf8Input(utf8InputLen + 1, '\0');

            if (utf8InputLen > 0)
            {
                ImTextStrToUtf8(utf8Input.data(), utf8InputLen + 1, io.InputQueueCharacters.Data, io.InputQueueCharacters.end());
                m_terminal->Write(utf8Input.data(), (size_t)utf8InputLen);
                io.InputQueueCharacters.resize(0);
            }
        }
    }

    else if (keyModFlags == ImGuiKeyModFlags_Ctrl)
    {
        constexpr auto asciiLen = (sizeof(asciiScancodeTable) / sizeof(asciiScancodeTable[0]));

        // I really dont like this, as it depends on the undelying backend implementation (SDL in this case)
        for (char i = 0; i < asciiLen; i++)
        {
            if (io.KeysDown[asciiScancodeTable[i]] && io.KeysDownDuration[asciiScancodeTable[i]] == 0.0f)
            {
                char tmp = i + 1;
                m_terminal->Write(&tmp, 1);
            }
        }
    }

    for (auto &kv : ImGuiTerminalKeyMap.Shortcuts())
    {
        ImGuiKey key = kv.first;
        auto keyIndex = io.KeyMap[key];
        if (io.KeysDown[keyIndex] && io.KeysDownDuration[keyIndex] == 0.0f)
        {
            for (auto &k : kv.second)
            {
                if ((k.mask == ImGuiKeyModFlags_Any || k.mask == keyModFlags) &&
                    (k.appkey == 0 || k.appkey < 0) && (k.appcursor == 0 || k.appcursor < 0))
                {
                    Action(k.action);
                    break;
                }
            }
        }
    }

    for (auto &kv : ImGuiTerminalKeyMap.KeyMap())
    {
        ImGuiKey key = kv.first;
        auto keyIndex = io.KeyMap[key];
        if (io.KeysDown[keyIndex] && io.KeysDownDuration[keyIndex] == 0.0f)
        {
            for (auto &k : kv.second)
            {
                if ((k.mask == ImGuiKeyModFlags_Any || k.mask == keyModFlags) &&
                    (k.appkey == 0 || k.appkey < 0) && (k.appcursor == 0 || k.appcursor < 0))
                {
                    auto keyString = k.string;
                    auto keyStringLen = strlen(keyString);
                    if (keyStringLen > 0)
                    {
                        m_terminal->Write(keyString, keyStringLen);
                    }
                    break;
                }
            }
        }
    }
    for (auto &kv : ImGuiTerminalKeyMap.PlatformKeyMap())
    {
        auto keyIndex = kv.first;
        if (io.KeysDown[keyIndex] && io.KeysDownDuration[keyIndex] == 0.0f)
        {
            for (auto &k : kv.second)
            {
                if ((k.mask == ImGuiKeyModFlags_Any || k.mask == keyModFlags) &&
                    (k.appkey == 0 || k.appkey < 0) && (k.appcursor == 0 || k.appcursor < 0))
                {
                    auto keyString = k.string;
                    auto keyStringLen = strlen(keyString);
                    if (keyStringLen > 0)
                    {
                        m_terminal->Write(keyString, keyStringLen);
                    }
                    break;
                }
            }
        }
    }
}

void ImGuiTerminal::Action(ShortcutAction action)
{
    if (action == ShortcutAction::PASTE)
    {
        auto clipboard = GetClipboard();
        auto clipboardLen = strlen(clipboard);
        if (clipboardLen > 0)
        {
            m_terminal->Write(clipboard, clipboardLen);
        }
    }
}

void ImGuiTerminal::SetClipboard(const char *text)
{
    ImGui::SetClipboardText(text);
}

const char *ImGuiTerminal::GetClipboard() const
{
#ifdef WIN32
    if (m_pasteNewlineFix)
    {
        std::string input = ImGui::GetClipboardText();
        decltype(input.find('\r')) pos;
        while ((pos = input.find('\r')) != std::string::npos)
        {
            input.erase(pos, 1);
        }
        m_clipboardLast = std::move(input);
        return m_clipboardLast.c_str();
    }
#endif
    return ImGui::GetClipboardText();
}

bool ImGuiTerminal::HasTerminated() const
{
    return m_terminal->HasExited();
}

void ImGuiTerminal::ResetColors()
{
    for (int i = 0; i < LEN(colornames); i++)
    {
        ResetColor(i, colornames[i]);
    }
}

static ImU32 ColorFromName(const char *s)
{
    if (s == nullptr || *s == '\0')
        return IM_COL32(0, 0, 0, 0xFF);

    if (*s == '#')
    {
        ++s;
        // Color is in #RGB(A) format
        auto len = strlen(s);
        if (len == 6)
        {
            // RGB
            int r, g, b;
            sscanf(s, "%02x%02x%02x", &r, &g, &b);
            return IM_COL32(r, g, b, 0xFF);
        }
        else if (len == 8)
        {
            // RGBA
            int r, g, b, a;
            sscanf(s, "%02x%02x%02x%02x", &r, &g, &b, &a);
            return IM_COL32(r, g, b, a);
        }
    }
    return 0;
}

int ImGuiTerminal::ResetColor(int index, const char *name)
{
    if (!name)
    {
        if (index >= 0 && index < m_colors.size())
        {
            ImU32 col = 0x000000FF;

            if (index < 256)
                col = colormapped[index];

            m_colors[index].first = col;
            m_colors[index].second = "";
            return 0;
        }
    }

    if (index >= 0 && index < m_colors.size())
    {
        m_colors[index].first = ColorFromName(name);
        m_colors[index].second = name;
    }

    return 1;
}

void ImGuiTerminal::SetTitle(const char *str)
{
    m_title = str ? str : "";
}

const std::string &ImGuiTerminal::GetTitle() const
{
    return m_title;
}

bool ImGuiTerminal::DrawBegin(int columns, int rows)
{
    if (columns != m_columns || rows != m_rows)
    {
        Hexe::Terminal::Glyph defaultGlyph;
        m_buffer.resize(columns * rows, defaultGlyph);
        m_columns = columns;
        m_rows = rows;
    }
    m_checkDirty = false;

    return ((m_mode & MODE_VISIBLE) != 0);
}

void ImGuiTerminal::DrawLine(Hexe::Terminal::Line line, int x1, int y, int x2)
{
    m_checkDirty = true;
    memcpy(&m_buffer[y * m_columns + x1], line, (x2 - x1) * sizeof(Glyph));
    for (int i = x1; i < x2; i++)
    {
        if (m_terminal->selected(i, y))
        {
            m_buffer[y * m_columns + i].mode |= ATTR_REVERSE;
        }
    }
}

void ImGuiTerminal::DrawCursor(int cx, int cy, Hexe::Terminal::Glyph g, int ox, int oy, Hexe::Terminal::Glyph og)
{
    m_cursorx = cx;
    m_cursory = cy;
    m_cursorg = g;
}

void ImGuiTerminal::DrawEnd()
{
    if (m_checkDirty)
        m_dirty = true;
}

void ImGuiTerminal::SetFont(ImFont *regular, ImFont *bold, ImFont *italic, ImFont *boldItalic)
{
    m_defaultFont = regular;
    m_boldFont = bold;
    m_italicFont = italic;
    m_boldItalicFont = boldItalic;
}

void ImGuiTerminal::Draw(ImDrawList *draw_list, ImVec2 pos, float scale, const ImVec4 &clip_rect, bool hasFocus)
{
    if (m_defaultFont == nullptr)
    {
        m_defaultFont = ImGui::GetDefaultFont();
    }

    if (m_elapsedTime - m_lastBlink > 0.7)
    {
        m_mode ^= MODE_BLINK;
        m_lastBlink = m_elapsedTime;
    }

    auto fontSize = m_defaultFont->FontSize * scale;
    auto advanceX = m_defaultFont->FindGlyph('A')->AdvanceX * scale;

    auto width = m_columns * advanceX;
    auto height = m_rows * fontSize;

    auto clipWidth = clip_rect.z - clip_rect.x;
    auto clipHeight = clip_rect.w - clip_rect.y;

    auto clipColumns = (int)std::floor(std::max(1.0f, clipWidth / advanceX));
    auto clipRows = (int)std::floor(std::max(1.0f, clipHeight / fontSize));

    if (width < clipWidth)
    {
        pos.x = std::floor(pos.x + ((clipWidth - width) / 2.0f));
    }
    if (height < clipHeight)
    {
        pos.y = std::floor(pos.y + ((clipHeight - height) / 2.0f));
    }

    ImGui::ItemAdd(ImRect{pos, ImVec2{pos.x + clipWidth, pos.y + clipHeight}}, ImGui::GetID("TermIO"));

    ImDrawList *drawList = ImGui::GetWindowDrawList();

    draw_list->AddRectFilled(ImVec2(clip_rect.x, clip_rect.y), ImVec2(clip_rect.z, clip_rect.w), GetCol(m_emulator->GetDefaultBackground(), m_colors), 0.0f, ImDrawCornerFlags_None);

    DrawImGui(draw_list, pos, scale, clip_rect);

    if (clipColumns != m_terminal->GetNumColumns() || clipRows != m_terminal->GetNumRows())
    {
        m_terminal->Resize(clipColumns, clipRows);
    }

    if (hasFocus)
    {
        auto mousePos = ImGui::GetMousePos();
        auto relPos = ImVec2{mousePos.x - pos.x, mousePos.y - pos.y};
        int mouseX = 0;
        int mouseY = 0;

        if (mousePos.x <= 0.0f || mousePos.y <= 0.0f)
        {
            mouseX = 0;
            mouseY = 0;
        }
        else if (relPos.x >= 0.0f && relPos.y >= 0.0f)
        {
            mouseX = ImClamp((int)std::floor(relPos.x / advanceX), 0, clipColumns);
            mouseY = ImClamp((int)std::floor(relPos.y / fontSize), 0, clipRows);
        }

        ProcessInput(mouseX, mouseY);
    }
}

void ImGuiTerminal::DrawImGui(ImDrawList *draw_list, ImVec2 pos, float scale, const ImVec4 &clip_rect)
{
    auto &io = ImGui::GetIO();
    auto font = m_defaultFont;

    ImDrawList *drawList = ImGui::GetWindowDrawList();
    auto fontSize = font->FontSize;
    auto spaceChar = font->FindGlyph('A');
    auto spaceCharAdvanceX = spaceChar->AdvanceX * scale;

    const float line_height = fontSize * scale;

    float x = 0.0f;
    float y = std::floor(pos.y);

    ImU32 col = 0xFFFFFFFF;

    // 4 vertices per primitive, up to 11 primitives pr char (background, glyph (up to 8 because of boxdraw), strikethrough, underline) and the cursor
    const int vtx_count_max = (int)(m_columns * m_rows) * 4 * 11 + 16;
    // 6 indices per primitive, up to 4 primitives pr char (background, glyph, strikethrough, underline) and the cursor
    const int idx_count_max = (int)(m_columns * m_rows) * 6 * 11 + 24;
    const int idx_expected_size = drawList->IdxBuffer.Size + idx_count_max;
    drawList->PrimReserve(idx_count_max, vtx_count_max);

    ImDrawVert *vtx_write = drawList->_VtxWritePtr;
    ImDrawIdx *idx_write = drawList->_IdxWritePtr;
    unsigned int vtx_current_idx = drawList->_VtxCurrentIdx;

    auto defaultFg = GetCol(m_emulator->GetDefaultForeground(), m_colors);
    auto defaultBg = GetCol(m_emulator->GetDefaultBackground(), m_colors);

    for (int j = 0; j < m_rows; j++)
    {
        x = std::floor(pos.x);

        if (pos.y + line_height * j > clip_rect.w)
            break;

        for (int i = 0; i < m_columns; i++)
        {
            auto &glyph = m_buffer[j * m_columns + i];
            auto fg = GetCol(glyph.fg, m_colors);
            auto bg = GetCol(glyph.bg, m_colors);
            ImU32 temp = 0;

            if (m_boldFont == nullptr && (glyph.mode & ATTR_BOLD_FAINT) == ATTR_BOLD && BETWEEN(glyph.fg, 0, 7))
                fg = GetCol(glyph.fg + 8, m_colors);

            if (IS_SET(MODE_REVERSE))
            {
                if (fg == defaultFg)
                {
                    fg = defaultBg;
                }
                else
                {
                    ImU32 red = (fg >> IM_COL32_R_SHIFT) & 0xFF;
                    ImU32 green = (fg >> IM_COL32_G_SHIFT) & 0xFF;
                    ImU32 blue = (fg >> IM_COL32_B_SHIFT) & 0xFF;
                    ImU32 alpha = (fg >> IM_COL32_A_SHIFT) & 0xFF;

                    fg = IM_COL32((~red) & 0xFF, (~green) & 0xFF, (~blue) & 0xFF, alpha);
                }

                if (bg == defaultBg)
                {
                    bg = defaultFg;
                }
                else
                {
                    ImU32 red = (bg >> IM_COL32_R_SHIFT) & 0xFF;
                    ImU32 green = (bg >> IM_COL32_G_SHIFT) & 0xFF;
                    ImU32 blue = (bg >> IM_COL32_B_SHIFT) & 0xFF;
                    ImU32 alpha = (bg >> IM_COL32_A_SHIFT) & 0xFF;

                    bg = IM_COL32((~red) & 0xFF, (~green) & 0xFF, (~blue) & 0xFF, alpha);
                }
            }

            if ((glyph.mode & ATTR_BOLD_FAINT) == ATTR_FAINT)
            {
                ImU32 red = (fg >> IM_COL32_R_SHIFT) & 0xFF;
                ImU32 green = (fg >> IM_COL32_G_SHIFT) & 0xFF;
                ImU32 blue = (fg >> IM_COL32_B_SHIFT) & 0xFF;
                ImU32 alpha = (fg >> IM_COL32_A_SHIFT) & 0xFF;

                red /= 2;
                green /= 2;
                blue /= 2;

                fg = IM_COL32(red, green, blue, alpha);
            }

            if (glyph.mode & ATTR_REVERSE)
            {
                temp = fg;
                fg = bg;
                bg = temp;
            }

            if (glyph.mode & ATTR_BLINK && m_mode & MODE_BLINK)
                fg = bg;

            if (glyph.mode & ATTR_INVISIBLE)
                fg = bg;

            bool isWide = glyph.mode & ATTR_WIDE;

            const ImFontGlyph *fontGlyph = nullptr;

            if ((glyph.mode & ATTR_BOLD) && (glyph.mode & ATTR_ITALIC) && m_boldItalicFont)
            {
                fontGlyph = m_boldItalicFont->FindGlyphNoFallback(glyph.u);
            }
            if (!fontGlyph && (glyph.mode & ATTR_BOLD) && m_boldFont)
            {
                fontGlyph = m_boldFont->FindGlyphNoFallback(glyph.u);
            }
            if (!fontGlyph && (glyph.mode & ATTR_ITALIC) && m_italicFont)
            {
                fontGlyph = m_italicFont->FindGlyphNoFallback(glyph.u);
            }
            if (fontGlyph == nullptr)
            {
                fontGlyph = font->FindGlyphNoFallback(glyph.u);
            }
            auto advanceX = spaceCharAdvanceX * (isWide ? 2.0f : 1.0f);
            auto ascent = font->Ascent * scale;
            if (!fontGlyph)
                fontGlyph = font->FallbackGlyph;

            if (glyph.mode & ATTR_WDUMMY)
            {
                continue;
            }

            {
                ImVec2 a(x, y), c(x + advanceX, y + line_height);
                ImVec2 b(c.x, a.y), d(a.x, c.y), uv(drawList->_Data->TexUvWhitePixel);
                idx_write[0] = vtx_current_idx;
                idx_write[1] = (ImDrawIdx)(vtx_current_idx + 1);
                idx_write[2] = (ImDrawIdx)(vtx_current_idx + 2);
                idx_write[3] = vtx_current_idx;
                idx_write[4] = (ImDrawIdx)(vtx_current_idx + 2);
                idx_write[5] = (ImDrawIdx)(vtx_current_idx + 3);
                vtx_write[0].pos = a;
                vtx_write[0].uv = uv;
                vtx_write[0].col = bg;
                vtx_write[1].pos = b;
                vtx_write[1].uv = uv;
                vtx_write[1].col = bg;
                vtx_write[2].pos = c;
                vtx_write[2].uv = uv;
                vtx_write[2].col = bg;
                vtx_write[3].pos = d;
                vtx_write[3].uv = uv;
                vtx_write[3].col = bg;
                vtx_write += 4;
                vtx_current_idx += 4;
                idx_write += 6;
            }

            if (glyph.mode & ATTR_BOXDRAW && m_useBoxDrawing)
            {
                auto bd = boxdrawindex(&glyph);
                drawbox(x, y, advanceX, line_height, fg, bg, bd, vtx_write, idx_write, vtx_current_idx, drawList->_Data->TexUvWhitePixel);
            }
            else
            {
                bool glyphVisible = fontGlyph && fontGlyph->Visible && fg != bg;
                if (glyphVisible)
                {
                    if (glyph.mode & ATTR_EMOJI && m_useColorEmoji)
                    {
                        fg = 0xFFFFFFFF;
                    }

                    // We don't do a second finer clipping test on the Y axis as we've already skipped anything before clip_rect.y and exit once we pass clip_rect.w
                    float x1 = x + fontGlyph->X0 * scale;
                    float x2 = x + fontGlyph->X1 * scale;
                    float y1 = y + fontGlyph->Y0 * scale;
                    float y2 = y + fontGlyph->Y1 * scale;
                    if (x1 <= clip_rect.z && x2 >= clip_rect.x)
                    {
                        // Render a character
                        float u1 = fontGlyph->U0;
                        float v1 = fontGlyph->V0;
                        float u2 = fontGlyph->U1;
                        float v2 = fontGlyph->V1;

                        // CPU side clipping used to fit text in their frame when the frame is too small. Only does clipping for axis aligned quads.
                        if (/*cpu_fine_clip*/ true)
                        {
                            if (x1 < clip_rect.x)
                            {
                                u1 = u1 + (1.0f - (x2 - clip_rect.x) / (x2 - x1)) * (u2 - u1);
                                x1 = clip_rect.x;
                            }
                            if (y1 < clip_rect.y)
                            {
                                v1 = v1 + (1.0f - (y2 - clip_rect.y) / (y2 - y1)) * (v2 - v1);
                                y1 = clip_rect.y;
                            }
                            if (x2 > clip_rect.z)
                            {
                                u2 = u1 + ((clip_rect.z - x1) / (x2 - x1)) * (u2 - u1);
                                x2 = clip_rect.z;
                            }
                            if (y2 > clip_rect.w)
                            {
                                v2 = v1 + ((clip_rect.w - y1) / (y2 - y1)) * (v2 - v1);
                                y2 = clip_rect.w;
                            }
                            if (y1 >= y2)
                            {
                                x += advanceX;
                                continue;
                            }
                        }

                        // We are NOT calling PrimRectUV() here because non-inlined causes too much overhead in a debug builds. Inlined here:
                        {
                            idx_write[0] = (ImDrawIdx)(vtx_current_idx);
                            idx_write[1] = (ImDrawIdx)(vtx_current_idx + 1);
                            idx_write[2] = (ImDrawIdx)(vtx_current_idx + 2);
                            idx_write[3] = (ImDrawIdx)(vtx_current_idx);
                            idx_write[4] = (ImDrawIdx)(vtx_current_idx + 2);
                            idx_write[5] = (ImDrawIdx)(vtx_current_idx + 3);
                            vtx_write[0].pos.x = x1;
                            vtx_write[0].pos.y = y1;
                            vtx_write[0].col = fg;
                            vtx_write[0].uv.x = u1;
                            vtx_write[0].uv.y = v1;
                            vtx_write[1].pos.x = x2;
                            vtx_write[1].pos.y = y1;
                            vtx_write[1].col = fg;
                            vtx_write[1].uv.x = u2;
                            vtx_write[1].uv.y = v1;
                            vtx_write[2].pos.x = x2;
                            vtx_write[2].pos.y = y2;
                            vtx_write[2].col = fg;
                            vtx_write[2].uv.x = u2;
                            vtx_write[2].uv.y = v2;
                            vtx_write[3].pos.x = x1;
                            vtx_write[3].pos.y = y2;
                            vtx_write[3].col = fg;
                            vtx_write[3].uv.x = u1;
                            vtx_write[3].uv.y = v2;
                            vtx_write += 4;
                            vtx_current_idx += 4;
                            idx_write += 6;
                        }
                    }
                }
            }

            if (glyph.mode & ATTR_UNDERLINE)
            {
                ImVec2 a(x, y + ascent + 1);
                ImVec2 c(x + advanceX, a.y + 1);
                ImVec2 b(c.x, a.y), d(a.x, c.y), uv(drawList->_Data->TexUvWhitePixel);
                idx_write[0] = vtx_current_idx;
                idx_write[1] = (ImDrawIdx)(vtx_current_idx + 1);
                idx_write[2] = (ImDrawIdx)(vtx_current_idx + 2);
                idx_write[3] = vtx_current_idx;
                idx_write[4] = (ImDrawIdx)(vtx_current_idx + 2);
                idx_write[5] = (ImDrawIdx)(vtx_current_idx + 3);
                vtx_write[0].pos = a;
                vtx_write[0].uv = uv;
                vtx_write[0].col = fg;
                vtx_write[1].pos = b;
                vtx_write[1].uv = uv;
                vtx_write[1].col = fg;
                vtx_write[2].pos = c;
                vtx_write[2].uv = uv;
                vtx_write[2].col = fg;
                vtx_write[3].pos = d;
                vtx_write[3].uv = uv;
                vtx_write[3].col = fg;
                vtx_write += 4;
                vtx_current_idx += 4;
                idx_write += 6;
            }

            if (glyph.mode & ATTR_STRUCK)
            {
                ImVec2 a(x, y + 2 * ascent / 3);
                ImVec2 c(x + advanceX, a.y + 1);
                ImVec2 b(c.x, a.y), d(a.x, c.y), uv(drawList->_Data->TexUvWhitePixel);
                idx_write[0] = vtx_current_idx;
                idx_write[1] = (ImDrawIdx)(vtx_current_idx + 1);
                idx_write[2] = (ImDrawIdx)(vtx_current_idx + 2);
                idx_write[3] = vtx_current_idx;
                idx_write[4] = (ImDrawIdx)(vtx_current_idx + 2);
                idx_write[5] = (ImDrawIdx)(vtx_current_idx + 3);
                vtx_write[0].pos = a;
                vtx_write[0].uv = uv;
                vtx_write[0].col = fg;
                vtx_write[1].pos = b;
                vtx_write[1].uv = uv;
                vtx_write[1].col = fg;
                vtx_write[2].pos = c;
                vtx_write[2].uv = uv;
                vtx_write[2].col = fg;
                vtx_write[3].pos = d;
                vtx_write[3].uv = uv;
                vtx_write[3].col = fg;
                vtx_write += 4;
                vtx_current_idx += 4;
                idx_write += 6;
            }

            x = x + advanceX;
        }

        y = y + line_height;
    }

    {
        if (!IS_SET(MODE_HIDE))
        {
            ImU32 drawcol;

            m_cursorg.mode &= ATTR_BOLD | ATTR_ITALIC | ATTR_UNDERLINE | ATTR_STRUCK | ATTR_WIDE | ATTR_BOXDRAW;
            if (IS_SET(MODE_REVERSE))
            {
                m_cursorg.mode |= ATTR_REVERSE;
                m_cursorg.bg = m_emulator->GetDefaultForeground();
                if (m_emulator->IsSelected(m_cursorx, m_cursory))
                {
                    drawcol = GetCol(m_emulator->GetDefaultCursorColor(), m_colors);
                    m_cursorg.fg = m_emulator->GetDefaultReverseCursorColor();
                }
                else
                {
                    drawcol = GetCol(m_emulator->GetDefaultReverseCursorColor(), m_colors);
                    m_cursorg.fg = m_emulator->GetDefaultCursorColor();
                }
            }
            else
            {
                if (m_emulator->IsSelected(m_cursorx, m_cursory))
                {
                    m_cursorg.fg = m_emulator->GetDefaultForeground();
                    m_cursorg.bg = m_emulator->GetDefaultReverseCursorColor();
                }
                else
                {
                    m_cursorg.fg = m_emulator->GetDefaultBackground();
                    m_cursorg.bg = m_emulator->GetDefaultCursorColor();
                }
                drawcol = GetCol(m_cursorg.bg, m_colors);
            }

            ImVec2 a{}, b{}, c{}, d{}, uv(drawList->_Data->TexUvWhitePixel);

            auto borderpx = m_borderpx * scale;
            auto cursorthickness = m_cursorthickness * scale;

            /* draw the new one */
            if (IS_SET(MODE_FOCUSED))
            {
                switch (m_cursorMode)
                {
                case 7:                   /* st extension */
                    m_cursorg.u = 0x2603; /* snowman (U+2603) */
                                          /* FALLTHROUGH */
                case 0:                   /* Blinking Block */
                case 1:                   /* Blinking Block (Default) */
                case 2:                   /* Steady Block */
                    // TODO: Implement cursor glyph rendering
                    //xdrawglyph(g, cx, cy);
                    //break;
                case 3: /* Blinking Underline */
                case 4: /* Steady Underline */
                    drawrect(drawcol, pos.x + borderpx + m_cursorx * spaceCharAdvanceX,
                             pos.y + borderpx + (m_cursory + 1) * line_height - cursorthickness,
                             spaceCharAdvanceX,
                             cursorthickness, vtx_write, idx_write, vtx_current_idx, uv);
                    break;
                case 5: /* Blinking bar */
                case 6: /* Steady bar */
                    drawrect(drawcol, pos.x + m_cursorx * spaceCharAdvanceX,
                             pos.y + m_cursory * line_height,
                             spaceCharAdvanceX, line_height,
                             vtx_write, idx_write, vtx_current_idx, uv);
                    break;
                }
            }
            else
            {
                drawrect(drawcol, pos.x + borderpx + m_cursorx * spaceCharAdvanceX,
                         pos.y + borderpx + m_cursory * line_height,
                         spaceCharAdvanceX - scale, scale, vtx_write, idx_write, vtx_current_idx, uv);
                drawrect(drawcol, pos.x + borderpx + m_cursorx * spaceCharAdvanceX,
                         pos.y + borderpx + m_cursory * line_height,
                         scale, line_height - scale,
                         vtx_write, idx_write, vtx_current_idx, uv);
                drawrect(drawcol, pos.x + borderpx + (m_cursorx + 1) * spaceCharAdvanceX - scale,
                         pos.y + borderpx + m_cursory * line_height,
                         scale, line_height - scale,
                         vtx_write, idx_write, vtx_current_idx, uv);
                drawrect(drawcol, pos.x + borderpx + (m_cursorx + 1) * spaceCharAdvanceX - scale,
                         pos.y + borderpx + m_cursory * line_height,
                         scale, line_height - scale,
                         vtx_write, idx_write, vtx_current_idx, uv);
                drawrect(drawcol, pos.x + borderpx + m_cursorx * spaceCharAdvanceX,
                         pos.y + borderpx + (m_cursory + 1) * line_height - scale,
                         spaceCharAdvanceX, scale,
                         vtx_write, idx_write, vtx_current_idx, uv);
            }
        }
    };

    // Give back unused vertices (clipped ones, blanks) ~ this is essentially a PrimUnreserve() action.
    drawList->VtxBuffer.Size = (int)(vtx_write - drawList->VtxBuffer.Data); // Same as calling shrink()
    drawList->IdxBuffer.Size = (int)(idx_write - drawList->IdxBuffer.Data);
    drawList->CmdBuffer[drawList->CmdBuffer.Size - 1].ElemCount -= (idx_expected_size - drawList->IdxBuffer.Size);
    drawList->_VtxWritePtr = vtx_write;
    drawList->_IdxWritePtr = idx_write;
    drawList->_VtxCurrentIdx = vtx_current_idx;

    m_elapsedTime += io.DeltaTime;
}

void ImGuiTerminal::Draw(const ImVec4 &contentArea, float scale)
{
    //bool hasFocus = true; //ImGui::IsItemFocused();
    bool hasFocus = ImGui::IsWindowFocused();

    bool modeFocus = m_mode & MODE_FOCUSED;
    if (hasFocus != modeFocus)
    {
        if (hasFocus)
        {
            m_mode |= MODE_FOCUSED | MODE_FOCUS;
        }
        else
        {
            m_mode ^= MODE_FOCUS | MODE_FOCUSED;
        }
    }
    else
    {
        m_mode ^= MODE_FOCUS;
    }

    auto *drawList = ImGui::GetWindowDrawList();
    Draw(drawList, ImVec2(contentArea.x, contentArea.y), scale, contentArea, hasFocus);
}

std::shared_ptr<ImGuiTerminal> ImGuiTerminal::Create(std::shared_ptr<Hexe::Terminal::TerminalEmulator> &&terminalEmulator, ImGuiTerminalConfig *config)
{
    std::shared_ptr<ImGuiTerminal> terminal = std::shared_ptr<ImGuiTerminal>(new ImGuiTerminal(terminalEmulator->GetNumColumns(), terminalEmulator->GetNumRows(), config));
    terminal->m_terminal = std::move(terminalEmulator);
    return terminal;
}

static Hexe::System::IProcessFactory *g_processFactory = new Hexe::System::ProcessFactory();

std::shared_ptr<ImGuiTerminal> ImGuiTerminal::Create(int columns, int rows, const std::string &program, const ImVector<std::string> &args, const std::string &workingDir, uint32_t options, System::IProcessFactory *processFactory)
{
    using namespace Hexe::System;

    ImGuiTerminalConfig config{};
    config.options = options;

    if (processFactory == nullptr)
    {
        processFactory = g_processFactory;
    }

    std::unique_ptr<IPseudoTerminal> pseudoTerminal = nullptr;
    std::vector<std::string> argsV(args.begin(), args.end());
    auto process = processFactory->CreateWithPseudoTerminal(program, argsV, workingDir, columns, rows, pseudoTerminal);

    if (!pseudoTerminal)
    {
        fprintf(stderr, "Failed to create pseudo terminal\n");
        return nullptr;
    }

    if (!process)
    {
        fprintf(stderr, "Failed to spawn process\n");
        return nullptr;
    }

    std::shared_ptr<ImGuiTerminal> terminal = std::shared_ptr<ImGuiTerminal>(new ImGuiTerminal(columns, rows, &config));
    terminal->m_terminal = TerminalEmulator::Create(std::move(pseudoTerminal), std::move(process), terminal);
    return terminal;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif