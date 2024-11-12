cmake_minimum_required(VERSION 3.28)

if(PLATFORM STREQUAL "SIMULATOR64"
   OR PLATFORM STREQUAL "OS64"
   OR PLATFORM STREQUAL "OS64COMBINED"
)
  set(IOS_BUILD TRUE)
endif()

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
if(NOT IOS_BUILD)
  find_package(OpenGL REQUIRED)
endif()

setup_dependencies()

# ---- Create library ----
add_executable(${PROJECT_NAME})

if(IOS_BUILD)
  target_sources(${PROJECT_NAME} PRIVATE ./iOS/LaunchScreen.storyboard)
  set_source_files_properties(
    ./iOS/LaunchScreen.storyboard PROPERTIES MACOSX_PACKAGE_LOCATION Resources
  )
endif()

set(MACOSX_BUNDLE_LONG_VERSION_STRING ${CMAKE_PROJECT_VERSION})
set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${CMAKE_PROJECT_VERSION})
set(MACOSX_BUNDLE_BUNDLE_VERSION ${CMAKE_PROJECT_VERSION})

if(CMAKE_GENERATOR STREQUAL Xcode)
  set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES MACOSX_BUNDLE TRUE
               MACOSX_BUNDLE_INFO_PLIST ./iOS/Info.plist
               # Xcode attributes for General tab
               XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.mycompany.unique.myapp"
               XCODE_ATTRIBUTE_PRODUCT_NAME ${PROJECT_NAME}
               XCODE_ATTRIBUTE_CURRENT_PROJECT_VERSION ${CMAKE_PROJECT_VERSION}
               XCODE_ATTRIBUTE_MARKETING_VERSION ${CMAKE_PROJECT_VERSION}
               XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2" # 1 for iPhone, 2 for iPad
               XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED YES
  )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
  # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or
  # SDL2::SDL2-static)
  target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2-static imgui::imgui)
if(IOS_BUILD)
  target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGLES")
  target_compile_definitions(${PROJECT_NAME} PRIVATE IOS_BUILD)
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

add_subdirectory(src)

# Group sources
_utils_group_target_sources(${PROJECT_NAME})

if(WIN32) # Copy SDL2.dll to output folder on Windows
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL2::SDL2>"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    VERBATIM
  )
endif()
