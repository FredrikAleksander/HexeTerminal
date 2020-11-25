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
#pragma once

#include "Hexe/Terminal/TerminalDisplay.h"
#include "Hexe/Terminal/TerminalEmulator.h"
#include "Hexe/System/IProcessFactory.h"
#include <memory>
#include <string>
#include <utility>
#include "imgui.h"

namespace Hexe
{
    namespace Terminal
    {
        enum class ShortcutAction
        {
            PASTE
        };

        enum ImGuiTerminalOptions
        {
            OPTION_NONE = 0,
            OPTION_COLOR_EMOJI = 1 << 0,
            OPTION_NO_BOXDRAWING = 1 << 1,
            OPTION_PASTE_CRLF = 1 << 2
        };

        struct ImGuiTerminalConfig
        {
            int options;
        };

        class ImGuiTerminal : public Hexe::Terminal::TerminalDisplay
        {
        private:
            float m_borderpx;
            float m_cursorthickness;
            int m_cursorx;
            int m_cursory;
            Hexe::Terminal::Glyph m_cursorg;
            int m_columns;
            int m_rows;
            bool m_dirty;
            bool m_checkDirty;
            uint32_t m_flags;
            bool m_useBoxDrawing;
            bool m_useColorEmoji;
            bool m_pasteNewlineFix;
            double m_elapsedTime;
            double m_lastBlink;

            ImFont *m_defaultFont;
            ImFont *m_boldFont;
            ImFont *m_italicFont;
            ImFont *m_boldItalicFont;

            struct
            {
                int sx;
                int sy;
                bool buttonDown[5];
            } m_mouseState;

            std::string m_title;
            ImVector<Hexe::Terminal::Glyph> m_buffer;
            ImVector<std::pair<ImU32, std::string>> m_colors;
            std::shared_ptr<Hexe::Terminal::TerminalEmulator> m_terminal;
            mutable std::string m_clipboardLast;

        private:
            void DrawImGui(ImDrawList *draw_list, ImVec2 pos, float scale, const ImVec4 &clip_rect);
            void Draw(ImDrawList *draw_list, ImVec2 pos, float scale, const ImVec4 &clip_rect, bool hasFocus);
            void ProcessInput(int mousecx, int mousecy);
            void MouseReport(int cx, int cy, int button, int state, int type);
            void Action(ShortcutAction action);

            ImGuiTerminal(int columns, int rows, ImGuiTerminalConfig *config);

        public:
            bool HasTerminated() const;

            virtual void ResetColors() override;
            virtual int ResetColor(int index, const char *name) override;

            virtual void SetTitle(const char *title) override;
            virtual const std::string &GetTitle() const;

            virtual void SetClipboard(const char *text);
            virtual const char *GetClipboard() const;

            virtual void SetFont(ImFont *regular, ImFont *bold = nullptr, ImFont *italic = nullptr, ImFont *boldItalic = nullptr);
            inline ImFont *GetFont() const { return m_defaultFont; }
            inline ImFont *GetFontBold() const { return m_boldFont; }
            inline ImFont *GetFontItalic() const { return m_italicFont; }
            inline ImFont *GetFontBoldItalic() const { return m_boldItalicFont; }

            virtual bool DrawBegin(int columns, int rows) override;
            virtual void DrawLine(Hexe::Terminal::Line line, int x1, int y, int x2) override;
            virtual void DrawCursor(int cx, int cy, Hexe::Terminal::Glyph g, int ox, int oy, Hexe::Terminal::Glyph og) override;
            virtual void DrawEnd() override;

            virtual void Update();
            void Draw(const ImVec4 &contentArea, float scale = 1.0f);

            static std::shared_ptr<ImGuiTerminal> Create(std::shared_ptr<Hexe::Terminal::TerminalEmulator> &&terminalEmulator, ImGuiTerminalConfig *config = 0);
            static std::shared_ptr<ImGuiTerminal> Create(int columns, int rows, const std::string &program, const ImVector<std::string> &args, const std::string &workingDir, uint32_t options = 0, System::IProcessFactory *processFactory = nullptr);
        };
    } // namespace Terminal
} // namespace Hexe