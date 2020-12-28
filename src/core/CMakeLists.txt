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

add_library(${NAME} STATIC ${OS_FILES}
  Core.hpp Core/Base.hpp Core/Log.cpp Core/Log.hpp Core/Assert.hpp Core/Debug/Instrumentor.hpp
  Core/FileSystem.cpp Core/FileSystem.hpp Core/Environment.hpp
  Core/Errors/ErrorReporter.cpp Core/Errors/ErrorReporter.hpp Core/Errors/Error.hpp
  Core/Errors/ErrorHandler.cpp Core/Errors/ErrorHandler.hpp
  Core/Config/ConfigFileResolver.cpp Core/Config/ConfigFileResolver.hpp
  Core/Config/ConfigLoader.cpp Core/Config/ConfigLoader.hpp
  Core/Config/Command.hpp Core/Config/Parameter.hpp
  Core/Config/CommandBuilder.cpp Core/Config/CommandBuilder.hpp
  Core/Config/ParameterBuilder.cpp Core/Config/ParameterBuilder.hpp
  Core/CLI/Shell.cpp Core/CLI/Shell.hpp)

target_include_directories(${NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src/core)
target_compile_features(${NAME} PRIVATE cxx_std_17)
target_link_libraries(${NAME}
  PRIVATE project_warnings
  PUBLIC fmt spdlog toml11 tsl::ordered_map)
