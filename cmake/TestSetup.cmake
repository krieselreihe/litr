# Test runner

add_library(Tests OBJECT ${PROJECT_SOURCE_DIR}/src/tests/Tests/Runner.cpp)
target_compile_features(Tests PRIVATE cxx_std_17)
target_link_libraries(Tests PUBLIC doctest)

# Base test setup

add_library(TestBase STATIC
  ${PROJECT_SOURCE_DIR}/src/tests/Helpers/TOML.cpp
  ${PROJECT_SOURCE_DIR}/src/tests/Helpers/TOML.hpp)
target_compile_features(TestBase PRIVATE cxx_std_17)
target_link_libraries(TestBase PUBLIC doctest fmt toml11 tsl::ordered_map Core)
target_include_directories(TestBase PUBLIC ${PROJECT_SOURCE_DIR}/src/tests)
