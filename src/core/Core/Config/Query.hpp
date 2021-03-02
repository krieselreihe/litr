#pragma once

#include <deque>
#include <string>
#include <vector>

#include "Core/Base.hpp"
#include "Core/Config/Loader.hpp"

namespace Litr::Config {

class Query {
 public:
  explicit Query(const Ref<Loader>& config);

  [[nodiscard]] Ref<Command> GetCommand(const std::string& name) const;
  [[nodiscard]] Ref<Parameter> GetParameter(const std::string& name) const;

 private:
  [[nodiscard]] static std::deque<std::string> SplitCommandQuery(const std::string& query);
  [[nodiscard]] static Ref<Command> GetCommandByPath(std::deque<std::string>& names, const std::vector<Ref<Command>>& commands);
  [[nodiscard]] static Ref<Command> GetCommandByName(const std::string& name, const std::vector<Ref<Command>>& commands);

 private:
  const Ref<Loader>& m_Config;
};

}  // namespace Litr::Config
