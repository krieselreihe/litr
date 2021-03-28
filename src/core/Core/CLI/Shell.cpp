#include "Shell.hpp"

#include <cstdio>

#include "Core/Log.hpp"
#include "Core/Debug/Instrumentor.hpp"

namespace Litr::CLI {

Shell::Result Shell::Exec(const std::string& command) {
  LITR_PROFILE_FUNCTION();

  return Shell::Exec(command, []([[maybe_unused]] const std::string& _buffer) {});
}

Shell::Result Shell::Exec(const std::string& command, const Path& path) {
  LITR_PROFILE_FUNCTION();

  return Shell::Exec(command, path, []([[maybe_unused]] const std::string& _buffer) {});
}

Shell::Result Shell::Exec(const std::string& command, const Shell::ExecCallback& callback) {
  LITR_PROFILE_FUNCTION();

  return Shell::Exec(command, Path(), callback);
}

Shell::Result Shell::Exec(const std::string& command, const Path& path, const Shell::ExecCallback& callback) {
  LITR_PROFILE_FUNCTION();

  Result result{};
  std::string cmd{CreateCommandString(command, path)};

  LITR_CORE_TRACE("Executing command \"{}\"", cmd);

  FILE* stream{popen(cmd.c_str(), "r")};

  if (stream) {
    const int max_buffer{256};
    char buffer[max_buffer];

    while (!feof(stream)) {
      if (fgets(buffer, max_buffer, stream) != nullptr) {
        result.Message.append(buffer);
        callback(std::string(buffer));
      }
    }

    result.Status = GetStatusCode(pclose(stream));
  }

  return result;
}

ExitStatus Shell::GetStatusCode(int streamStatus) {
  return static_cast<ExitStatus>(streamStatus / 256);
}

std::string Shell::CreateCommandString(const std::string& command, const Path& path) {
  LITR_PROFILE_FUNCTION();

  std::string mainCommand{command};
  mainCommand.append(" 2>&1");

  if (path.Empty()) {
    return mainCommand;
  }

  // Change current working directory
  std::string cmd{CreateCdCommand(path)};
  cmd.append(" && ").append(mainCommand);

  // Reset current working directory
  Path returnPath{};
  for (size_t i{0}; i < path.Count(); ++i) {
    returnPath = returnPath.Append(std::string(".."));
  }

  cmd.append(" && ").append(CreateCdCommand(returnPath));

  return cmd;
}

std::string Shell::CreateCdCommand(const Path& path) {
  LITR_PROFILE_FUNCTION();

  std::string cmd{"cd "};
  cmd.append(path.ToString());
  return cmd;
}

}  // namespace Litr::CLI
