# --- Hooks ---

add_executable(Hooks_Help Hooks/Help.unit.cpp $<TARGET_OBJECTS:Tests>
  # @todo: Because the client is an executable, files needed for a test
  #  need to be explicitly declared here. This could be done better.
  ../../../client/Client/Hooks/Hook.hpp
  ../../../client/Client/Hooks/Help/NewHelp.cpp
  ../../../client/Client/Hooks/Help/NewHelp.hpp
  ../../../client/Client/Hooks/Help/HelpStrategy.hpp
  ../../../client/Client/Hooks/Help/HelpAllStrategy.cpp
  ../../../client/Client/Hooks/Help/HelpAllStrategy.hpp
  ../../../client/Client/Hooks/Help/HelpCommandStrategy.cpp
  ../../../client/Client/Hooks/Help/HelpCommandStrategy.hpp)
add_test(NAME Hooks_Help COMMAND Hooks_Help)
target_link_libraries(Hooks_Help PRIVATE ClientTestBase)
