# Test runner

add_library(Tests OBJECT ${CMAKE_SOURCE_DIR}/src/tests/Tests/Runner.cpp)
target_compile_features(Tests PRIVATE cxx_std_17)
target_link_libraries(Tests PUBLIC doctest)

# Base test setup

add_library(TestBase INTERFACE)
target_sources(TestBase INTERFACE
  ${CMAKE_SOURCE_DIR}/src/tests/Helpers/TOML.cpp
  ${CMAKE_SOURCE_DIR}/src/tests/Helpers/TOML.hpp)
target_compile_features(TestBase INTERFACE cxx_std_17)
target_link_libraries(TestBase INTERFACE doctest fmt toml11)
target_include_directories(TestBase INTERFACE ${CMAKE_SOURCE_DIR}/src/tests)
