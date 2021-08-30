cmake_minimum_required(VERSION 3.15)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(
      FATAL_ERROR
      "In-source builds are not supported. "
      "Make a new directory (e.g., 'build'), and run CMake from there. "
      "You may need to delete 'CMakeCache.txt' and 'CMakeFiles/' first."
  )
endif()

project(
    tupl
    VERSION 0.2.0
    DESCRIPTION "C++20 aggregate tuple"
    HOMEPAGE_URL "https://github.com/willwray/tupl"
    LANGUAGES CXX
)

# This variable is set by project() in CMake 3.21+
string(
    COMPARE EQUAL
    "${CMAKE_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}"
    PROJECT_IS_TOP_LEVEL
)

# ---- Developer mode ----

# Developer mode enables targets and paths only relevant for developers.
# Targets needed to build the project must be provided unconditionally,
# so consumers can trivially build and package the project
if(PROJECT_IS_TOP_LEVEL)
  option(tupl_DEVELOPER_MODE "Enable developer mode" OFF)
endif()

# ---- Warning guard ----

# target_include_directories with SYSTEM modifier requests the compiler
# to omit warnings from the provided paths, if the compiler supports it.
# This is to provide a user experience similar to find_package when
# add_subdirectory or FetchContent is used to consume this project
set(tupl_warning_guard "")
if(NOT PROJECT_IS_TOP_LEVEL)
  option(
      tupl_INCLUDES_WITH_SYSTEM
      "Use SYSTEM modifier for tupl's includes, disabling warnings"
      ON
  )
  mark_as_advanced(tupl_INCLUDES_WITH_SYSTEM)
  if(tupl_INCLUDES_WITH_SYSTEM)
    set(tupl_warning_guard SYSTEM)
  endif()
endif()

# ---- Fetch dependencies; c_array_support & IREPEAT (for codegen) ----

include(FetchContent)

FetchContent_Declare(
  irepeat
  GIT_REPOSITORY https://github.com/willwray/IREPEAT
  GIT_TAG v0.3
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)

FetchContent_Declare(
  c_array_support
  GIT_REPOSITORY https://github.com/willwray/c_array_support
  GIT_TAG v0.5
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)

FetchContent_MakeAvailable(irepeat)

FetchContent_MakeAvailable(c_array_support)

add_library(irepeat INTERFACE)
target_include_directories(irepeat INTERFACE ${irepeat_SOURCE_DIR}/)

add_library(c_array_support INTERFACE)
target_include_directories(
            c_array_support INTERFACE ${c_array_support_SOURCE_DIR}/)

# ---- Declare library ----

add_library(tupl_tupl INTERFACE)
add_library(tupl::tupl ALIAS tupl_tupl)

target_link_libraries(tupl_tupl INTERFACE irepeat c_array_support)

set_property(
    TARGET tupl_tupl PROPERTY
    EXPORT_NAME tupl
)

target_include_directories(
    tupl_tupl ${tupl_warning_guard}
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
    "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>"
)

target_compile_features(tupl_tupl INTERFACE cxx_std_20)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_INSTALL_INCLUDEDIR tupl CACHE PATH "")
  endif()

  #include(CMakePackageConfigHelpers)
  #include(GNUInstallDirs)
  
  # find_package(<package>) call for consumers to find this project
  set(package tupl)

  install(
      DIRECTORY .
      DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
      COMPONENT tupl_Development
  )

  #install(
  #    TARGETS tupl_tupl
  #    EXPORT tuplTargets
  #    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  #)
  
  #write_basic_package_version_file(
  #    "${package}ConfigVersion.cmake"
  #    COMPATIBILITY SameMajorVersion
  #    ARCH_INDEPENDENT
  #)
  
  # Allow package maintainers to override the path for the configs
  set(
      tupl_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/${package}"
      CACHE PATH "CMake package config location relative to the "
                 "install prefix"
  )
  mark_as_advanced(tupl_INSTALL_CMAKEDIR)
  
  install(
      FILES cmake/install-config.cmake
      DESTINATION "${tupl_INSTALL_CMAKEDIR}"
      RENAME "${package}Config.cmake"
      COMPONENT tupl_Development
  )
  
  install(
      FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
      DESTINATION "${tupl_INSTALL_CMAKEDIR}"
      COMPONENT tupl_Development
  )
  
  #install(
  #    EXPORT tuplTargets
  #    NAMESPACE tupl::
  #    DESTINATION "${tupl_INSTALL_CMAKEDIR}"
  #    COMPONENT tupl_Development
  #)
  
  if(PROJECT_IS_TOP_LEVEL)
    include(CPack)
  endif()
endif()

# ---- Developer mode ----

if(NOT tupl_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of tupl"
  )
endif()

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
