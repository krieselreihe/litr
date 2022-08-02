# Generate universal executable for Apple hardware
# This file needs to be included before calling `project`.
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "")
endif ()
