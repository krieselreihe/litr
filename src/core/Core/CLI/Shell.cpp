#include "Shell.hpp"

#include <cstdio>
#include <functional>

namespace Litr {

Shell::Result Shell::Exec(const std::string& command) {
  return Shell::Exec(command, []([[maybe_unused]] const std::string& _buffer) {});
}

Shell::Result Shell::Exec(const std::string& command, const Shell::ExecCallback& callback) {
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

    // Dividing by 256 to get the status code
    int statusCode{pclose(stream) / 256};
    result.Status = statusCode;
  }

  return result;
}

}  // namespace Litr
