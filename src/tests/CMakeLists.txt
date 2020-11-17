set(NAME "Tests")

enable_testing()

include(${CMAKE_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

# Test runner

add_library(TestRunner OBJECT Tests/Runner.cpp)
target_compile_features(TestRunner PRIVATE cxx_std_17)
target_link_libraries(TestRunner PUBLIC doctest)

# Base test setup

add_library(TestBase INTERFACE)
target_compile_features(TestBase INTERFACE cxx_std_17)
target_link_libraries(TestBase INTERFACE doctest)

# Test cases

add_executable(ConfigFileTest
  Helpers/TOML.cpp Helpers/TOML.hpp
  Tests/Core/Config/ConfigFileResolver.unit.cpp
  Tests/Core/Config/ParameterBuilder.unit.cpp
  $<TARGET_OBJECTS:TestRunner>)

add_test(NAME ConfigFileTest COMMAND ConfigFileTest)
target_link_libraries(ConfigFileTest PRIVATE TestBase Core)
target_include_directories(TestBase INTERFACE
  ${CMAKE_SOURCE_DIR}/src/tests
  ${CMAKE_SOURCE_DIR}/src/core)
