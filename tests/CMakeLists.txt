cmake_minimum_required(VERSION 3.15)

project(tuplTests LANGUAGES CXX)

# This variable is set by project() in CMake 3.21+
string(
    COMPARE EQUAL
    "${CMAKE_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}"
    PROJECT_IS_TOP_LEVEL
)

# This function will add shared libraries to PATH when running the test
# so they can be found. Windows does not support RPATH or similar. See:
# https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-search-order
# Usage: windows_set_path(<test> <target>...)
function(windows_set_path TEST)
  if(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    return()
  endif()

  set(path "")
  set(glue "")
  foreach(target IN LISTS ARGN)
    get_target_property(type "${target}" TYPE)
    if(type STREQUAL "SHARED_LIBRARY")
      set(path "${path}${glue}$<TARGET_FILE_DIR:${target}>")
      set(glue "\;") # backslash is important
    endif()
  endforeach()
  if(NOT path STREQUAL "")
    set_property(TEST "${TEST}" PROPERTY ENVIRONMENT "PATH=${path}")
  endif()
endfunction()

if(PROJECT_IS_TOP_LEVEL)
  find_package(tupl REQUIRED)
  enable_testing()
endif()

add_executable(tupl_test tupl_test.cpp)
target_link_libraries(tupl_test PRIVATE tupl::tupl)
target_compile_features(tupl_test PRIVATE cxx_std_20)

add_test(NAME tupl_test COMMAND tupl_test)
