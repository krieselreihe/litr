#pragma once

#include <string>
#include <cstddef>
#include <cstring>
#include <vector>

namespace Litr::CLI {

class Instruction {
 public:
  using Value = std::string;

  enum class Code : unsigned char {
    CLEAR,        // Comma operator
    DEFINE,       // Define a variable
    CONSTANT,     // Set constant value
    BEGIN_SCOPE,  // Begin command scope
    EXECUTE       // Execute a command
  };

  Instruction() = default;
  explicit Instruction(std::vector<std::byte> data);

  void Write(Code code);
  void Write(std::byte byte);
  [[nodiscard]] std::byte Read(size_t offset) const;
  [[nodiscard]] size_t Count() const;

  std::byte WriteConstant(const Value& value);
  [[nodiscard]] Value ReadConstant(std::byte index) const;

 private:
  std::vector<std::byte> m_ByteCode{};
  std::vector<Value> m_Constants{};
};

}  // namespace Litr::CLI
