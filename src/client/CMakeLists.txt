set(NAME "Client")

include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_executable(${NAME} Client/Client.cpp Client/Application.cpp Client/Application.hpp)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(${NAME} PRIVATE cxx_std_17)
target_link_libraries(${NAME} PRIVATE project_warnings Core)
