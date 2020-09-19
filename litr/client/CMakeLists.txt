set(CMAKE_CXX_STANDARD 17)
set(NAME "Client")

include(${CMAKE_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_executable(${NAME} Client/Client.cpp)

target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lox/client)
target_link_libraries(${NAME} PRIVATE project_warnings)
