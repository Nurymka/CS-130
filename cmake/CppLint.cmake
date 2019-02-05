# Original code by Daniel Scharrer & Greg Horn
# https://github.com/acado/acado/blob/master/cmake/CppLint.cmake

find_package(PythonInterp)

# Add a target that runs cpplint.py
#
# Parameters:
# - TARGET_NAME the name of the target to add
# - SOURCES_LIST a complete list of source and include files to check
function(add_style_check_target)
  if(NOT PYTHONINTERP_FOUND)
    return()
  endif()

  cmake_parse_arguments(ARG "" "TARGET_NAME" "SOURCES_LIST" ${ARGN})

  list(REMOVE_DUPLICATES ARG_SOURCES_LIST)
  list(SORT ARG_SOURCES_LIST)

  add_custom_target(${ARG_TARGET_NAME}
  COMMAND "${CMAKE_COMMAND}" -E chdir
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "${PYTHON_EXECUTABLE}"
            "${CMAKE_SOURCE_DIR}/misc/cpplint.py"
            "--recursive"
            ${ARG_SOURCES_LIST}
    DEPENDS ${ARG_SOURCES_LIST}
    COMMENT "Linting")

endfunction()