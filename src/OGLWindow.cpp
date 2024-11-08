#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>
#include <imgui.h>

#include <OGLWindow.hpp>
#include <iostream>

namespace SDLTest {
  OGLWindow::OGLWindow(int width, int height) {
    SetupSDLWindow(width, height);
    SetupImGui();
  }

  OGLWindow::~OGLWindow() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(m_gl_context);
    SDL_DestroyWindow(m_pwindow);
    SDL_Quit();
  }

  void OGLWindow::run() {
    // Our state
    ImGuiIO& io = ImGui::GetIO();
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) done = true;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE
            && event.window.windowID == SDL_GetWindowID(m_pwindow))
          done = true;
      }
      if (SDL_GetWindowFlags(m_pwindow) & SDL_WINDOW_MINIMIZED) {
        SDL_Delay(10);
        continue;
      }

      // Start the Dear ImGui frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplSDL2_NewFrame();
      ImGui::NewFrame();

      // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can
      // browse its code to learn more about Dear ImGui!).
      if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

      // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named
      // window.
      {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin(
            "Hello, world!");  // Create a window called "Hello, world!" and append into it.

        ImGui::Text(
            "This is some useful text.");  // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window",
                        &show_demo_window);  // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f,
                           1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color",
                          (float*)&clear_color);  // Edit 3 floats representing a color

        if (ImGui::Button("Button"))  // Buttons return true when clicked (most widgets return true
                                      // when edited/activated)
          counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate,
                    io.Framerate);
        ImGui::End();
      }

      // 3. Show another simple window.
      if (show_another_window) {
        ImGui::Begin(
            "Another Window",
            &show_another_window);  // Pass a pointer to our bool variable (the window will have a
                                    // closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me")) show_another_window = false;
        ImGui::End();
      }

      // Rendering
      ImGui::Render();
      glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
      glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                   clear_color.z * clear_color.w, clear_color.w);
      glClear(GL_COLOR_BUFFER_BIT);
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      SDL_GL_SwapWindow(m_pwindow);
    }
  }

  void OGLWindow::SetupSDLWindow(int width, int height) {
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
      std::cout << "Error: " << SDL_GetError() << "\n";
      return;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    m_glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    m_glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);  // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    m_glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags
        = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    m_pwindow = SDL_CreateWindow("Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, width, height, window_flags);

    if (m_pwindow == nullptr) {
      std::cout << "Error: SDL_CreateWindow():" << SDL_GetError() << "\n";
      return;
    }

    m_gl_context = SDL_GL_CreateContext(m_pwindow);
    SDL_GL_MakeCurrent(m_pwindow, m_gl_context);
    // SDL_GL_SetSwapInterval(1); // Enable vsync
  }

  void OGLWindow::SetupImGui() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(m_pwindow, m_gl_context);
    ImGui_ImplOpenGL3_Init(m_glsl_version.c_str());
  }
}  // namespace SDLTest