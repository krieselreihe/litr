set(NAME "Config")

include(${CMAKE_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} Config/Config.cpp)

target_compile_features(${NAME} PRIVATE cxx_std_17)
target_link_libraries(${NAME} PRIVATE project_warnings tomlplusplus)
