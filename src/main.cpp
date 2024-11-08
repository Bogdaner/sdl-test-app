#include <OGLWindow.hpp>

// Main code
int main(int, char**) {
  {
    auto window = SDLTest::OGLWindow(1600, 900);
    window.run();
  }

  {
    auto window = SDLTest::OGLWindow(2000, 1200);
    window.run();
  }

  return 0;
}