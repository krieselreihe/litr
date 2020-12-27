set(NAME "Toms")

include(${CMAKE_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC Toms.hpp Toms/Value.cpp Toms/Value.hpp)

target_include_directories(${NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src/toms)
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(${NAME} PRIVATE cxx_std_17)
target_link_libraries(${NAME} PRIVATE project_warnings tsl::ordered_map toml11::toml11)
