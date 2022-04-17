set(NAME "Tests")

enable_testing()

include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/TestSetup.cmake)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Fixtures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(Tests/Core)
add_subdirectory(Tests/Client)
