# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)

  # Set the possible values of build type for cmake-gui, ccmake
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

find_program(CCACHE ccache)
if(CCACHE)
  message(STATUS "Using ccache")
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
else()
  message(STATUS "Ccache not found")
endif()

# Generate compile_commands.json to make it easier to work with clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(DEACTIVATE_LOGGING "Disable logging" OFF)
if (DEACTIVATE_LOGGING)
  add_compile_definitions(LITR_DEACTIVATE_LOGGING)
endif()

option(TRACE "Enable detailed execution flow tracing" OFF)
if (TRACE)
  add_compile_definitions(TRACE)
endif()

option(DEBUG "Enable debug statements and asserts" OFF)
if (DEBUG)
  add_compile_definitions(DEBUG LITR_ENABLE_ASSERTS)
endif()

option(PROFILE "Enable profiling tools" OFF)
if (PROFILE)
  add_compile_definitions(LITR_PROFILE)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  add_compile_definitions(LITR_PLATFORM_WINDOWS)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_compile_definitions(LITR_PLATFORM_LINUX)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  add_compile_definitions(LITR_PLATFORM_MACOS)
endif()
