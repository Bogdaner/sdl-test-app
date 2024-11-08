#ifndef OGLWINDOW_HPP
#define OGLWINDOW_HPP

#pragma once

#include <SDL.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>
#include <imgui.h>

#include <string>

#if defined(IMGUI_IMPL_OPENGL_ES2)
  #include <SDL_opengles2.h>
#else
  #include <SDL_opengl.h>
#endif

namespace SDLTest {
  class OGLWindow {
  public:
    OGLWindow(int width, int height);
    ~OGLWindow();

    void run();

  protected:
  private:
    void SetupSDLWindow(int width, int height);
    void SetupImGui();

    SDL_Window* m_pwindow = nullptr;
    SDL_GLContext m_gl_context;
    std::string m_glsl_version;
  };
}  // namespace SDLTest

#endif  // OGLWINDOW_HPP
