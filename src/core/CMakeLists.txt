set(NAME "Core")

include(${CMAKE_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

# Define set of OS specific files to include
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(OS_FILES "Platform/WindowsEnvironment.cpp")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(OS_FILES "Platform/LinuxEnvironment.cpp")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(OS_FILES "Platform/MacEnvironment.cpp")
endif()

add_library(${NAME}
  Core.hpp Core/Base.hpp Core/Log.cpp Core/Log.hpp Core/Assert.hpp
  Core/Debug/Instrumentor.hpp
  ${OS_FILES}
  Core/ConfigFile.cpp Core/ConfigFile.hpp Core/Environment.hpp Core/FileSystem.cpp Core/FileSystem.hpp)

target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/core)

target_compile_features(${NAME} PRIVATE cxx_std_17)
target_link_libraries(${NAME}
  PRIVATE project_warnings
  PUBLIC fmt spdlog tomlplusplus)
