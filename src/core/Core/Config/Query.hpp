#pragma once

#include <deque>
#include <string>
#include <vector>
#include <unordered_map>

#include "Core/Base.hpp"
#include "Core/Config/Loader.hpp"
#include "Core/CLI/Variable.hpp"

namespace Litr::Config {

class Query {
  using Parts = std::deque<std::string>;
  using Variables = std::unordered_map<std::string, CLI::Variable>;

 public:
  using Commands = std::vector<Ref<Command>>;
  using Parameters = std::vector<Ref<Parameter>>;

  explicit Query(const Ref<Loader>& config);

  [[nodiscard]] Ref<Command> GetCommand(const std::string& name) const;
  [[nodiscard]] Ref<Parameter> GetParameter(const std::string& name) const;

  [[nodiscard]] Commands GetCommands() const;
  [[nodiscard]] Parameters GetParameters() const;

  [[nodiscard]] std::vector<std::string> GetUsedCommandParameters(const std::string& name) const;

 private:
  [[nodiscard]] static Parts SplitCommandQuery(const std::string& query);
  [[nodiscard]] static Ref<Command> GetCommandByPath(Parts& names, const Loader::Commands& commands);
  [[nodiscard]] static Ref<Command> GetCommandByName(const std::string& name, const Loader::Commands& commands);

  [[nodiscard]] Variables GetParametersAsVariables() const;

 private:
  const Ref<Loader>& m_Config;
};

}  // namespace Litr::Config
