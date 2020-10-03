set(NAME "tomlplusplus")

project(${NAME})

add_library(${NAME} INTERFACE)
target_compile_features(${NAME} INTERFACE cxx_std_17)
target_include_directories(${NAME} INTERFACE .)
