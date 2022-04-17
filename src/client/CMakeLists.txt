set(NAME "Client")

include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_executable(${NAME}
  Client/Client.cpp Client/Application.cpp Client/Application.hpp
  Client/Hooks/Handler.cpp Client/Hooks/Handler.hpp Client/Hooks/Hook.hpp
  Client/Hooks/Help.cpp Client/Hooks/Help.hpp
  Client/Hooks/Version.cpp Client/Hooks/Version.hpp
  Client/Hooks/Help/NewHelp.cpp Client/Hooks/Help/NewHelp.hpp
  Client/Hooks/Help/HelpStrategy.hpp
  Client/Hooks/Help/HelpAllStrategy.cpp Client/Hooks/Help/HelpAllStrategy.hpp
  Client/Hooks/Help/HelpCommandStrategy.cpp Client/Hooks/Help/HelpCommandStrategy.hpp)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(${NAME} PRIVATE cxx_std_17)
target_link_libraries(${NAME} PRIVATE project_warnings Core)
