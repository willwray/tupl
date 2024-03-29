cmake_minimum_required(VERSION 3.15)

# Contents generated by 'cmake-init -h', manually edited to a standalone
# single file, with includes expanded inline

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(
      FATAL_ERROR
      "In-source builds are not supported. "
      "Make a new directory (e.g., 'build'), and run CMake from there. "
      "You may need to delete 'CMakeCache.txt' and 'CMakeFiles/' first."
  )
endif()

project(
    tupl_dev
    VERSION 0.3.0
    DESCRIPTION "Code generation for tupl project"
    LANGUAGES CXX
)

# PROJECT_IS_TOP_LEVEL variable is set by project() in CMake 3.21+
string(
    COMPARE EQUAL
    "${CMAKE_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}"
    PROJECT_IS_TOP_LEVEL
)

# ---- Fetch dependency; IREPEAT (for codegen) ----

include(FetchContent)

FetchContent_Declare(
  irepeat
  GIT_REPOSITORY https://github.com/willwray/IREPEAT
  GIT_TAG v0.3
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)

FetchContent_MakeAvailable(irepeat)

add_library(irepeat INTERFACE)
target_include_directories(irepeat INTERFACE ${irepeat_SOURCE_DIR}/)

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
