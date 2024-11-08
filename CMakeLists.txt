cmake_minimum_required(VERSION 3.28)

# ---- CMake Includes ----
include(cmake/utils.cmake)
include(cmake/dependencies.cmake)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# ---- Project ----
project(
  sdl-test-app
  VERSION 1.0
  LANGUAGES CXX
)

# ---- Third Parties ----
find_package(OpenGL REQUIRED)

setup_dependencies()

# ---- Create library ----
add_executable(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
  # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or
  # SDL2::SDL2-static)
  target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 imgui::imgui OpenGL::GL)

target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

add_subdirectory(src)

# Group sources
_utils_group_target_sources(${PROJECT_NAME})

if(WIN32)
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL2::SDL2>"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    VERBATIM
  )
endif()
