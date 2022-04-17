# Test runner

add_library(Tests OBJECT ${PROJECT_SOURCE_DIR}/src/tests/Tests/Runner.cpp)
target_compile_features(Tests PRIVATE cxx_std_17)
target_link_libraries(Tests PUBLIC doctest)

# Base client test setup

add_library(ClientTestBase INTERFACE)
target_compile_features(ClientTestBase INTERFACE cxx_std_17)
target_link_libraries(ClientTestBase INTERFACE doctest fmt Core)
target_include_directories(ClientTestBase INTERFACE ${PROJECT_SOURCE_DIR}/src/tests)
target_include_directories(ClientTestBase INTERFACE ${PROJECT_SOURCE_DIR}/src/client)

# Base core test setup

add_library(CoreTestBase STATIC
  ${PROJECT_SOURCE_DIR}/src/tests/Helpers/TOML.cpp
  ${PROJECT_SOURCE_DIR}/src/tests/Helpers/TOML.hpp)
target_compile_features(CoreTestBase PRIVATE cxx_std_17)
target_link_libraries(CoreTestBase PUBLIC doctest fmt toml11 tsl::ordered_map Core)
target_include_directories(CoreTestBase PUBLIC ${PROJECT_SOURCE_DIR}/src/tests)
