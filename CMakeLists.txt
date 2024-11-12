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
if(PLATFORM STREQUAL "SIMULATOR64")

else()
  find_package(OpenGL REQUIRED)
endif()

setup_dependencies()

# ---- Create library ----
add_executable(${PROJECT_NAME})
set(MACOSX_BUNDLE_LONG_VERSION_STRING "1.0")
set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES MACOSX_BUNDLE TRUE
             MACOSX_BUNDLE_INFO_PLIST cmake/ios/Info.plist
             # New Xcode attributes for General tab
             XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.mycompany.myapp"
             XCODE_ATTRIBUTE_PRODUCT_NAME ${PROJECT_NAME}
             XCODE_ATTRIBUTE_CURRENT_PROJECT_VERSION ${CMAKE_PROJECT_VERSION}
             XCODE_ATTRIBUTE_MARKETING_VERSION ${CMAKE_PROJECT_VERSION}
)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
  # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or
  # SDL2::SDL2-static)
  target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 imgui::imgui)
if(PLATFORM STREQUAL "SIMULATOR64")
  target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGLES")
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
endif()

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
