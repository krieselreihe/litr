cmake_minimum_required(VERSION 3.17)

project(
  Litr
  VERSION 1.0.0
  DESCRIPTION "Language Independent Task Runner, configure and then run any tasks you want."
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include(cmake/StandardProjectSettings.cmake)

# Link this "library" to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

add_subdirectory(litr/client)
