function(_utils_group_target_sources target)
  get_target_property(sources ${target} SOURCES)
  message(STATUS "Target sources ${sources}")
  foreach(file ${sources})
    get_filename_component(path "${file}" ABSOLUTE)
    get_filename_component(path "${path}" PATH)
    if(file MATCHES "${PROJECT_BINARY_DIR}")
      file(RELATIVE_PATH path ${PROJECT_BINARY_DIR} "${path}")
    else()
      file(RELATIVE_PATH path ${PROJECT_SOURCE_DIR} "${path}")
    endif()
    string(REGEX REPLACE "/" "\\\\" win_path "${path}")
    string(REGEX REPLACE "^..\\\\" "" win_path "${win_path}")
    source_group("${win_path}" REGULAR_EXPRESSION "${path}/[^/\\]+\\..*")
  endforeach()
endfunction()

function(_utils_get_all_cmake_targets out_var current_dir)
  get_property(
    targets
    DIRECTORY ${current_dir}
    PROPERTY BUILDSYSTEM_TARGETS
  )
  get_property(
    subdirs
    DIRECTORY ${current_dir}
    PROPERTY SUBDIRECTORIES
  )

  foreach(subdir ${subdirs})
    _utils_get_all_cmake_targets(subdir_targets ${subdir})
    list(APPEND targets ${subdir_targets})
  endforeach()

  set(${out_var}
      ${targets}
      PARENT_SCOPE
  )
endfunction()
