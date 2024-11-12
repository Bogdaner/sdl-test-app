include(FetchContent)
include(cmake/utils.cmake)

function(setup_dependencies)

  # retrieve a copy of the current directory's `COMPILE_OPTIONS`
  get_directory_property(old_dir_compile_options COMPILE_OPTIONS)

  # modify the actual value of the current directory's `COMPILE_OPTIONS` (copy from above line
  # remains unchanged). subdirectories inherit a copy of their parent's `COMPILE_OPTIONS` at the
  # time the subdirectory is processed.
  add_compile_options(-w)

  # ---- SDL2 ----
  FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-2.30.9
  )
  FetchContent_MakeAvailable(SDL2)

  # ---- IMGUI ----
  FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.5
  )
  FetchContent_MakeAvailable(imgui)

  add_library(
    imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
  )
  add_library(imgui::imgui ALIAS imgui)
  set_target_properties(
    imgui
    PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${imgui_BINARY_DIR}
               LIBRARY_OUTPUT_DIRECTORY ${imgui_BINARY_DIR}
               RUNTIME_OUTPUT_DIRECTORY ${imgui_BINARY_DIR}
  )
  set_target_properties(imgui PROPERTIES CXX_STANDARD 17)

  if(PLATFORM STREQUAL "SIMULATOR64")
    target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_ES2)
  endif()

  # SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
  if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or
    # SDL2::SDL2-static)
    target_link_libraries(imgui PRIVATE SDL2::SDL2main)
  endif()
  target_link_libraries(imgui PRIVATE SDL2::SDL2)
  target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})

  # ---- Group targets to make generated VS, Xcode solutions cleaner ----
  _utils_get_all_cmake_targets(SDL2_TARGETS ${SDL2_SOURCE_DIR})
  set(IMGUI_TARGETS "imgui") # Imgui doesn't contain CMakeLists.txt by default and it target is
                             # defined here
  set(PROJECT_THIRD_PARTY_TARGETS "${SDL2_TARGETS}" "${IMGUI_TARGETS}")

  foreach(target ${PROJECT_THIRD_PARTY_TARGETS})
    set_target_properties(${target} PROPERTIES FOLDER "3rdParty")
  endforeach()

  # restore the current directory's old `COMPILE_OPTIONS`
  set_directory_properties(PROPERTIES COMPILE_OPTIONS "${old_dir_compile_options}")

endfunction()
