/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>
#include <cstddef>
#include <cstring>
#include <vector>

namespace litr::cli {

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

  void write(Code code);
  void write(std::byte byte);
  [[nodiscard]] std::byte read(size_t offset) const;
  [[nodiscard]] size_t count() const;

  std::byte write_constant(const Value& value);
  [[nodiscard]] Value read_constant(std::byte index) const;

 private:
  std::vector<std::byte> m_byte_code{};
  std::vector<Value> m_constants{};
};

}  // namespace litr::cli
