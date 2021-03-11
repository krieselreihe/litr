#pragma once

// Base -------------------------------

#include "Core/Assert.hpp"
#include "Core/Base.hpp"
#include "Core/Log.hpp"
#include "Core/Utils.hpp"

// Main -------------------------------

#include "Core/Environment.hpp"
#include "Core/FileSystem.hpp"

// Config -----------------------------

#include "Core/Config/Command.hpp"
#include "Core/Config/FileResolver.hpp"
#include "Core/Config/Loader.hpp"
#include "Core/Config/Parameter.hpp"
#include "Core/Config/Query.hpp"

// CLI --------------------------------

#include "Core/CLI/Shell.hpp"
#include "Core/CLI/Token.hpp"
#include "Core/CLI/Scanner.hpp"
#include "Core/CLI/Parser.hpp"
#include "Core/CLI/Instruction.hpp"
#include "Core/CLI/Interpreter.hpp"

// Script -----------------------------

#include "Core/Script/Parser.hpp"

// Errors -----------------------------

#include "Core/Error/BaseError.hpp"
#include "Core/Error/Reporter.hpp"
#include "Core/Error/Handler.hpp"

// Debug ------------------------------

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Debug/Disassembler.hpp"
