#include <GL/glew.h>
#include <SDL.h>
#include <filesystem>
#include <fstream>
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include "../imgui_freetype_ex.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "Hexe/Terminal/ImGuiTerminal.h"

#undef max
#undef min

struct TerminalOptions
{
    std::string program;
    ImVector<std::string> arguments;

    std::string font;
    std::string fontBold;
    std::string fontItalic;
    std::string fontBoldItalic;
    std::string fontEmoji;

    float fontSize;

    int windowWidth;
    int windowHeight;
    bool fullscreen;
};

static void LoadEmojiFont(const std::string &emojiFontPath, ImVector<unsigned char> &emojiBuffer)
{
    if (std::filesystem::exists(emojiFontPath))
    {
        std::ifstream emojiFile(emojiFontPath.c_str(), std::ios::binary | std::ios::ate);
        std::streamsize size = emojiFile.tellg();
        emojiFile.seekg(0, std::ios::beg);

        emojiBuffer.resize((int)size);
        if (!emojiFile.read((char *)emojiBuffer.Data, size))
        {
            emojiBuffer.clear();
        }
    }
}

void SetDefaultFont(TerminalOptions &options, const std::filesystem::path &basePath)
{
    using std::filesystem::exists;

    auto fontDefault = basePath / "JetBrains Mono Regular Nerd Font Complete Mono.ttf";
    auto fontBold = basePath / "JetBrains Mono Italic Nerd Font Complete Mono.ttf";
    auto fontItalic = basePath / "JetBrains Mono Bold Nerd Font Complete Mono.ttf";
    auto fontBoldItalic = basePath / "JetBrains Mono Bold Italic Nerd Font Complete Mono.ttf";
    auto emojiFontPath = basePath / "NotoColorEmoji.ttf";

    if (exists(fontDefault))
    {
        options.fontSize = 16;
        options.font = fontDefault.string();
        if (exists(fontBold))
            options.fontBold = fontBold.string();
        if (exists(fontItalic))
            options.fontItalic = fontItalic.string();
        if (exists(fontBoldItalic))
            options.fontBoldItalic = fontBoldItalic.string();
        if (exists(emojiFontPath))
            options.fontEmoji = emojiFontPath.string();
    }
}

int main(int argc, char *argv[])
{
    TerminalOptions options{};

    options.windowWidth = 800;
    options.windowHeight = 600;

    std::filesystem::path basePath{SDL_GetBasePath()};

    SetDefaultFont(options, basePath);

    if (options.fontSize <= 0)
    {
        options.fontSize = 13;
    }

    if (options.program.empty())
    {
#ifdef WIN32
        options.program = "cmd.exe";
#else
        options.program = "/bin/sh";
#endif
    }

#ifdef WIN32
    SetEnvironmentVariableA("WSLENV", "TERM/u");
    SetEnvironmentVariableA("TERM", "st-256color");
#endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        fprintf(stderr, "%s", SDL_GetError());
        return 1;
    }

    const char *glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (options.fullscreen)
    {
        window_flags = (SDL_WindowFlags)(window_flags | SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    SDL_Window *window = SDL_CreateWindow("Terminal", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, options.windowWidth, options.windowHeight, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    bool err = glewInit() != GLEW_OK;
    if (err)
    {
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    unsigned int rasterizerFlags = 0;

    const ImWchar powerlineRange[] = {0xe0a0, 0xe0d4, 0};
    const ImWchar deviconsRange[] = {0xe700, 0xe7c5, 0};
    const ImWchar setiuiRange[] = {0xe5fa, 0xe62b, 0};
    const ImWchar octiconsRange[] = {0xf400, 0xf67c, 0};
    const ImWchar fontlinuxRange[] = {0xf300, 0xf313, 0};

    // Add the symbol ranges found in Nerd Fonts
    ImVector<ImWchar> glyphRanges;
    ImFontGlyphRangesBuilder glyphRangeBuilder;
    glyphRangeBuilder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    glyphRangeBuilder.AddRanges(powerlineRange);
    glyphRangeBuilder.AddRanges(deviconsRange);
    glyphRangeBuilder.AddRanges(setiuiRange);
    glyphRangeBuilder.AddRanges(octiconsRange);
    glyphRangeBuilder.AddRanges(fontlinuxRange);
    glyphRangeBuilder.BuildRanges(&glyphRanges);

    ImFontConfig cfg{};
    cfg.SizePixels = options.fontSize;
    // cfg.OversampleH = 1;
    // cfg.OversampleV = 1;

    ImFont *fontDefault = nullptr;
    ImFont *fontBold = nullptr;
    ImFont *fontItalic = nullptr;
    ImFont *fontBoldItalic = nullptr;

    if (std::filesystem::exists(options.font))
    {
        rasterizerFlags = ImGuiFreeTypeEx::RasterizerFlags::ForceAutoHint;
        cfg.RasterizerFlags = rasterizerFlags | ImGuiFreeTypeEx::EmbedEmoji;

        fontDefault = io.Fonts->AddFontFromFileTTF(options.font.c_str(), options.fontSize, &cfg, glyphRanges.Data);

        cfg.RasterizerFlags = rasterizerFlags;

        if (std::filesystem::exists(options.fontBold))
            fontBold = io.Fonts->AddFontFromFileTTF(options.fontBold.c_str(), options.fontSize, &cfg);
        if (std::filesystem::exists(options.fontItalic))
            fontBold = io.Fonts->AddFontFromFileTTF(options.fontItalic.c_str(), options.fontSize, &cfg);
        if (std::filesystem::exists(options.fontBoldItalic))
            fontBold = io.Fonts->AddFontFromFileTTF(options.fontBoldItalic.c_str(), options.fontSize, &cfg);
    }
    else
    {
        cfg.RasterizerFlags = ImGuiFreeTypeEx::EmbedEmoji;
        io.Fonts->AddFontDefault(&cfg);
    }

    ImVector<unsigned char> emojiFontData{};
    LoadEmojiFont(options.fontEmoji, emojiFontData);
    ImGuiFreeTypeEx::BuildFontAtlas(io.Fonts, 0, emojiFontData);

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui_ImplOpenGL3_CreateFontsTexture();

    bool showDemoWindow = true;
    bool showTerminalWindow = true;

    bool exitRequested = false;

    std::string title = "Terminal";

    std::shared_ptr<Hexe::Terminal::ImGuiTerminal> terminal = nullptr;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!exitRequested)
    {
        SDL_Event event;
        if (SDL_WaitEventTimeout(&event, 4))
        {
            do
            {
                ImGui_ImplSDL2_ProcessEvent(&event);
                switch (event.type)
                {
                case SDL_QUIT:
                    exitRequested = true;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                        exitRequested = true;
                }
            } while (SDL_PollEvent(&event));
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit"))
                    exitRequested = true;
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Windows"))
            {
                ImGui::MenuItem("Demo Window", nullptr, &showDemoWindow);
                ImGui::MenuItem("Terminal Window", nullptr, &showTerminalWindow);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (terminal)
        {
            terminal->Update();
            if (terminal->GetTitle() != title)
            {
                title = terminal->GetTitle();
                SDL_SetWindowTitle(window, title.empty() ? "Terminal" : title.c_str());
            }
        }

        if (showDemoWindow)
        {
            ImGui::ShowDemoWindow(&showDemoWindow);
        }

        if (showTerminalWindow)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{});
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{});

            auto displaySize = io.DisplaySize;

            ImGui::SetNextWindowPos(ImVec2{});
            ImGui::SetNextWindowSize(displaySize);

            if (ImGui::Begin("Terminal", &showTerminalWindow, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
            {
                auto scale = ImGui::GetFontSize() / fontDefault->FontSize;

                auto contentRegion = ImGui::GetContentRegionAvail();
                auto contentPos = ImGui::GetCursorScreenPos();

                if (!terminal)
                {
                    auto spacingChar = fontDefault->FindGlyph('A');
                    auto charWidth = spacingChar->AdvanceX * scale;
                    auto charHeight = fontDefault->FontSize * scale;

                    auto columns = (int)std::floor(std::max(1.0f, contentRegion.x / charWidth));
                    auto rows = (int)std::floor(std::max(1.0f, contentRegion.y / charHeight));

                    terminal = Hexe::Terminal::ImGuiTerminal::Create(columns, rows, options.program, options.arguments, "", emojiFontData.empty() ? 0 : Hexe::Terminal::ImGuiTerminal::OPTION_COLOR_EMOJI);
                    terminal->SetFont(fontDefault, fontBold, fontItalic, fontBoldItalic);
                }
                if (!terminal || terminal->HasTerminated())
                    exitRequested = true;
                else
                {
                    terminal->Draw(ImVec4(contentPos.x, contentPos.y, contentPos.x + contentRegion.x, contentPos.y + contentRegion.y), scale);
                }
            }
            ImGui::End();

            ImGui::PopStyleVar(2);
        }

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
        // if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        // {
        //     SDL_Window *backup_current_window = SDL_GL_GetCurrentWindow();
        //     SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
        //     ImGui::UpdatePlatformWindows();
        //     ImGui::RenderPlatformWindowsDefault();
        //     SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        // }

        SDL_GL_SwapWindow(window);
        SDL_Delay(1);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    if (gl_context)
    {
        SDL_GL_DeleteContext(gl_context);
    }
    if (window)
    {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();
    return 0;
}