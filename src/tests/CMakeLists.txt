set(NAME "Tests")

enable_testing()

include(${CMAKE_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/TestSetup.cmake)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Fixtures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(Tests/Core)
