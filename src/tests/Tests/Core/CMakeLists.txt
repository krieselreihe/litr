# --- Config ---

add_executable(Config_Loader Config/Loader.int.cpp $<TARGET_OBJECTS:Tests>)
add_test(NAME Config_Loader COMMAND Config_Loader)
target_link_libraries(Config_Loader PRIVATE TestBase Core)

add_executable(Config_FileResolver Config/FileResolver.unit.cpp $<TARGET_OBJECTS:Tests>)
add_test(NAME Config_FileResolver COMMAND Config_FileResolver)
target_link_libraries(Config_FileResolver PRIVATE TestBase Core)

add_executable(Config_CommandBuilder Config/CommandBuilder.unit.cpp $<TARGET_OBJECTS:Tests>)
add_test(NAME Config_CommandBuilder COMMAND Config_CommandBuilder)
target_link_libraries(Config_CommandBuilder PRIVATE TestBase Core)

add_executable(Config_ParameterBuilder Config/ParameterBuilder.unit.cpp $<TARGET_OBJECTS:Tests>)
add_test(NAME Config_ParameterBuilder COMMAND Config_ParameterBuilder)
target_link_libraries(Config_ParameterBuilder PRIVATE TestBase Core)

add_executable(Config_Query Config/Query.int.cpp $<TARGET_OBJECTS:Tests>)
add_test(NAME Config_Query COMMAND Config_Query)
target_link_libraries(Config_Query PRIVATE TestBase Core)

# --- CLI ---

add_executable(CLI_Scanner CLI/Scanner.unit.cpp $<TARGET_OBJECTS:Tests>)
add_test(NAME CLI_Scanner COMMAND CLI_Scanner)
target_link_libraries(CLI_Scanner PRIVATE TestBase Core)

add_executable(CLI_Parser CLI/Parser.unit.cpp $<TARGET_OBJECTS:Tests>)
add_test(NAME CLI_Parser COMMAND CLI_Parser)
target_link_libraries(CLI_Parser PRIVATE TestBase Core)

# --- Script ---

add_executable(Script_Scanner Script/Scanner.unit.cpp $<TARGET_OBJECTS:Tests>)
add_test(NAME Script_Scanner COMMAND Script_Scanner)
target_link_libraries(Script_Scanner PRIVATE TestBase Core)

add_executable(Script_Compiler Script/Compiler.unit.cpp $<TARGET_OBJECTS:Tests>)
add_test(NAME Script_Compiler COMMAND Script_Compiler)
target_link_libraries(Script_Compiler PRIVATE TestBase Core)
