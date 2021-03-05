set(NAME "Core")

include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
  Core/Debug/Instrumentor.hpp Core/Debug/Disassembler.cpp Core/Debug/Disassembler.hpp
  Core.hpp Core/Base.hpp Core/Log.cpp Core/Log.hpp Core/Assert.hpp
  Core/FileSystem.cpp Core/FileSystem.hpp Core/Environment.hpp
  Core/Utils.cpp Core/Utils.hpp
  Core/Errors/ErrorReporter.cpp Core/Errors/ErrorReporter.hpp Core/Errors/Error.hpp
  Core/Errors/ErrorHandler.cpp Core/Errors/ErrorHandler.hpp
  Core/Config/FileResolver.cpp Core/Config/FileResolver.hpp
  Core/Config/Loader.cpp Core/Config/Loader.hpp
  Core/Config/Command.hpp Core/Config/Parameter.hpp
  Core/Config/CommandBuilder.cpp Core/Config/CommandBuilder.hpp
  Core/Config/ParameterBuilder.cpp Core/Config/ParameterBuilder.hpp
  Core/CLI/Shell.cpp Core/CLI/Shell.hpp Core/CLI/Parser.cpp Core/CLI/Parser.hpp
  Core/CLI/Scanner.cpp Core/CLI/Scanner.hpp Core/CLI/Token.hpp
  Core/CLI/Instruction.cpp Core/CLI/Instruction.hpp
  Core/Config/Query.cpp Core/Config/Query.hpp)

# Define set of OS specific files to include
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_sources(${NAME} PRIVATE Platform/WindowsEnvironment.cpp)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_sources(${NAME} PRIVATE Platform/LinuxEnvironment.cpp)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  target_sources(${NAME} PRIVATE Platform/MacEnvironment.cpp)
endif()

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(${NAME} PRIVATE cxx_std_17)
target_link_libraries(${NAME}
  PRIVATE project_warnings
  PUBLIC fmt spdlog toml11 tsl::ordered_map)
