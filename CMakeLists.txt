cmake_minimum_required(VERSION 3.15)

project(
  Litr
  DESCRIPTION "Language Independent Task Runner lets you configure and run any tasks you want for any language."
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/StandardProjectSettings.cmake)
include(cmake/SetVersion.cmake)

# Link this "library" to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

add_subdirectory(vendor)
add_subdirectory(src)
