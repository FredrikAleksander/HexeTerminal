#include <GL/glew.h>
#include <SDL.h>
#include <filesystem>
#include <fstream>
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include "../imgui_freetype_ex.h"
#include "Hexe/Terminal/ImGuiTerminal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

#undef max
#undef min

static void LoadEmojiFont(ImVector<unsigned char> &emojiBuffer)
{

  auto p = std::filesystem::path(SDL_GetBasePath()) / "NotoColorEmoji.ttf";
  if (std::filesystem::exists(p))
  {
    std::ifstream emojiFile(p.c_str(), std::ios::binary | std::ios::ate);
    std::streamsize size = emojiFile.tellg();
    emojiFile.seekg(0, std::ios::beg);

    emojiBuffer.resize((int)size);
    if (!emojiFile.read((char *)emojiBuffer.Data, size))
    {
      emojiBuffer.clear();
    }
  }
}

int main(int argc, char *argv[])
{
  int windowWidth = 800;
  int windowHeight = 600;
  bool fullscreen = false;

#ifdef WIN32
  SetEnvironmentVariableA("WSLENV", "TERM/u");
  SetEnvironmentVariableA("TERM", "st-256color");
#endif

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) !=
      0)
  {
    fprintf(stderr, "%s", SDL_GetError());
    return 1;
  }

  const char *glsl_version = "#version 150";
  SDL_GL_SetAttribute(
      SDL_GL_CONTEXT_FLAGS,
      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(
      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  if (fullscreen)
  {
    window_flags =
        (SDL_WindowFlags)(window_flags | SDL_WINDOW_FULLSCREEN_DESKTOP);
  }
  SDL_Window *window = SDL_CreateWindow(
      "Simple Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      windowWidth, windowHeight, window_flags);
  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1); // Enable vsync

  glewExperimental = true;

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

  ImFontConfig cfg{};
  // cfg.SizePixels = 26;
  // cfg.OversampleH = 1;
  // cfg.OversampleV = 1;
  cfg.RasterizerFlags = ImGuiFreeTypeEx::EmbedEmoji;
  // auto fontDefault = io.Fonts->AddFontFromFileTTF(options.font.c_str(),
  // options.fontSize != 0.0f ? options.fontSize : 23.0f, &cfg,
  // glyphRanges.Data);
  auto fontDefault = io.Fonts->AddFontDefault(&cfg);

  ImVector<unsigned char> emojiFontData{};
  LoadEmojiFont(emojiFontData);
  ImGuiFreeTypeEx::BuildFontAtlas(io.Fonts, 0, emojiFontData);

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);
  ImGui_ImplOpenGL3_CreateFontsTexture();

  bool showDemoWindow = true;
  bool showTerminalWindow = true;

  bool exitRequested = false;

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
          if (event.window.event == SDL_WINDOWEVENT_CLOSE &&
              event.window.windowID == SDL_GetWindowID(window))
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
      terminal->Update();

    if (showDemoWindow)
    {
      ImGui::ShowDemoWindow(&showDemoWindow);
    }

    if (showTerminalWindow)
    {
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{});
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{});

      if (ImGui::Begin("Terminal", &showTerminalWindow,
                       ImGuiWindowFlags_NoMove))
      {
        auto scale = ImGui::GetFontSize() / fontDefault->FontSize;

        auto contentRegion = ImGui::GetContentRegionAvail();
        auto contentPos = ImGui::GetCursorScreenPos();

        if (!terminal || terminal->HasTerminated())
        {
          auto spacingChar = fontDefault->FindGlyph('A');
          auto charWidth = spacingChar->AdvanceX * scale;
          auto charHeight = fontDefault->FontSize * scale;

          auto columns =
              (int)std::floor(std::max(1.0f, contentRegion.x / charWidth));
          auto rows =
              (int)std::floor(std::max(1.0f, contentRegion.y / charHeight));

          terminal = Hexe::Terminal::ImGuiTerminal::Create(
              columns, rows, "cmd.exe", {}, "",
              emojiFontData.empty()
                  ? 0
                  : Hexe::Terminal::ImGuiTerminalOptions::OPTION_COLOR_EMOJI);
        }
        if (!terminal)
          exitRequested = true;
        else
        {
          terminal->Draw(ImVec4(contentPos.x, contentPos.y,
                                contentPos.x + contentRegion.x,
                                contentPos.y + contentRegion.y),
                         scale);
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
    // (Platform functions may change the current OpenGL context, so we
    // save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call SDL_GL_MakeCurrent(window,
    //  gl_context) directly)
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
