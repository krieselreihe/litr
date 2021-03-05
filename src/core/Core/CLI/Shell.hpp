#pragma once

#include <string>
#include <functional>

namespace Litr::CLI {

class Shell {
 public:
  struct Result {
    int Status{EXIT_SUCCESS};
    std::string Message{};
  } __attribute__((aligned(32)));

  using ExecCallback = std::function<void(const std::string&)>;

  [[nodiscard]] static Result Exec(const std::string& command);
  static Result Exec(const std::string& command, const Shell::ExecCallback& callback);

 private:
  static int GetStatusCode(int streamStatus);
};

}  // namespace Litr::CLI
