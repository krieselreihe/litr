set(NAME "Config")

include(${CMAKE_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME}
  Config.hpp
  Config/ConfigFile.cpp Config/ConfigFile.hpp)

target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/litr/config)

target_compile_features(${NAME} PRIVATE cxx_std_17)
target_link_libraries(${NAME} PRIVATE project_warnings fmt tomlplusplus)
