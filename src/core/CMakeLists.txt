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
  Core/ConfigFileResolver.cpp Core/ConfigFileResolver.hpp Core/FileSystem.cpp Core/FileSystem.hpp
  Core/Configuration.cpp Core/Environment.hpp Core/Configuration.hpp
  Core/Command.hpp Core/Parameter.hpp
  Core/Errors/ErrorReporter.cpp Core/Errors/ErrorReporter.hpp Core/Errors/ConfigurationError.hpp Core/CommandBuilder.cpp Core/CommandBuilder.hpp)

target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/core)

target_compile_features(${NAME} PRIVATE cxx_std_17)
target_link_libraries(${NAME}
  PRIVATE project_warnings
  PUBLIC fmt spdlog toml11)
