set(NAME "Client")

include(${CMAKE_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_executable(${NAME} Client/Client.cpp Client/Application.cpp Client/Application.hpp)

target_include_directories(${NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src/client)
target_compile_features(${NAME} PRIVATE cxx_std_17)
target_link_libraries(${NAME} PRIVATE project_warnings Core)
