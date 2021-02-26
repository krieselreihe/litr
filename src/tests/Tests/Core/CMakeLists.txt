# --- Config ---

add_executable(Config_ConfigLoader Config/ConfigLoader.int.cpp $<TARGET_OBJECTS:Tests>)
add_test(NAME Config_ConfigLoader COMMAND Config_ConfigLoader)
target_link_libraries(Config_ConfigLoader PRIVATE TestBase Core)

add_executable(Config_ConfigFileResolver Config/ConfigFileResolver.unit.cpp $<TARGET_OBJECTS:Tests>)
add_test(NAME Config_ConfigFileResolver COMMAND Config_ConfigFileResolver)
target_link_libraries(Config_ConfigFileResolver PRIVATE TestBase Core)

add_executable(Config_CommandBuilder Config/CommandBuilder.unit.cpp $<TARGET_OBJECTS:Tests>)
add_test(NAME Config_CommandBuilder COMMAND Config_CommandBuilder)
target_link_libraries(Config_CommandBuilder PRIVATE TestBase Core)

add_executable(Config_ParameterBuilder Config/ParameterBuilder.unit.cpp $<TARGET_OBJECTS:Tests>)
add_test(NAME Config_ParameterBuilder COMMAND Config_ParameterBuilder)
target_link_libraries(Config_ParameterBuilder PRIVATE TestBase Core)

# --- CLI ---

add_executable(CLI_Scanner CLI/Scanner.unit.cpp $<TARGET_OBJECTS:Tests>)
add_test(NAME CLI_Scanner COMMAND CLI_Scanner)
target_link_libraries(CLI_Scanner PRIVATE TestBase Core)

add_executable(CLI_Parser CLI/Parser.unit.cpp $<TARGET_OBJECTS:Tests>)
add_test(NAME CLI_Parser COMMAND CLI_Parser)
target_link_libraries(CLI_Parser PRIVATE TestBase Core)
