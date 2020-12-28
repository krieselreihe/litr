set(NAME "Tests")

enable_testing()

include(${CMAKE_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Fixtures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Test runner

add_library(TestRunner OBJECT Tests/Runner.cpp)
target_compile_features(TestRunner PRIVATE cxx_std_17)
target_link_libraries(TestRunner PUBLIC doctest)

# Base test setup

add_library(TestBase INTERFACE)
target_compile_features(TestBase INTERFACE cxx_std_17)
target_link_libraries(TestBase INTERFACE doctest)

# Test cases

# --- ConfigLoader ---

add_executable(ConfigLoader Tests/Core/Config/ConfigLoader.int.cpp $<TARGET_OBJECTS:TestRunner>)
add_test(NAME ConfigLoader COMMAND ConfigLoader)
target_link_libraries(ConfigLoader PRIVATE TestBase Core)

# --- ConfigFileResolver ---

add_executable(ConfigFileResolver Tests/Core/Config/ConfigFileResolver.unit.cpp $<TARGET_OBJECTS:TestRunner>)
add_test(NAME ConfigFileResolver COMMAND ConfigFileResolver)
target_link_libraries(ConfigFileResolver PRIVATE TestBase Core)

# --- CommandBuilder / ParameterBuilder ---

add_executable(CommandAndParameterBuilder
  Helpers/TOML.cpp Helpers/TOML.hpp
  Tests/Core/Config/CommandBuilder.unit.cpp
  Tests/Core/Config/ParameterBuilder.unit.cpp
  $<TARGET_OBJECTS:TestRunner>)

add_test(NAME CommandAndParameterBuilder COMMAND CommandAndParameterBuilder)
target_link_libraries(CommandAndParameterBuilder PRIVATE TestBase Core)
target_include_directories(TestBase INTERFACE ${CMAKE_SOURCE_DIR}/src/tests)

# --- CLI ---

add_executable(CLI Tests/Core/CLI/Scanner.unit.cpp $<TARGET_OBJECTS:TestRunner>)
add_test(NAME CLI COMMAND CLI)
target_link_libraries(CLI PRIVATE TestBase Core)
