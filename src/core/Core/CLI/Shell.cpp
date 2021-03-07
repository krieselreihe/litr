#include "Shell.hpp"

#include "Core/Debug/Instrumentor.hpp"

#include <cstdio>

namespace Litr::CLI {

Shell::Result Shell::Exec(const std::string& command) {
  LITR_PROFILE_FUNCTION();

  return Shell::Exec(command, []([[maybe_unused]] const std::string& _buffer) {});
}

Shell::Result Shell::Exec(const std::string& command, const Shell::ExecCallback& callback) {
  LITR_PROFILE_FUNCTION();

  Result result{};
  std::string cmd{command};
  cmd.append(" 2>&1");

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

}  // namespace Litr::CLI
