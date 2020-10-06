cmake_minimum_required(VERSION 3.17)

project(
  src
  VERSION 1.0.0
  DESCRIPTION "Language Independent Task Runner lets you configure and then run any tasks you want for any language."
  LANGUAGES CXX)

include(cmake/StandardProjectSettings.cmake)

# Link this "library" to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# Dependencies
add_subdirectory(vendor/doctest)
add_subdirectory(vendor/fmt)
add_subdirectory(vendor/tomlplusplus)

# Application libraries and files
add_subdirectory(src/tests)
add_subdirectory(src/config)
add_subdirectory(src/client)
