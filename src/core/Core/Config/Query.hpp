#pragma once

#include <deque>
#include <string>
#include <vector>

#include "Core/Base.hpp"
#include "Core/Config/Loader.hpp"

namespace Litr::Config {

class Query {
  using Parts = std::deque<std::string>;

 public:

  explicit Query(const Ref<Loader>& config);

  [[nodiscard]] Ref<Command> GetCommand(const std::string& name) const;
  [[nodiscard]] Ref<Parameter> GetParameter(const std::string& name) const;

 private:
  [[nodiscard]] static Parts SplitCommandQuery(const std::string& query);
  [[nodiscard]] static Ref<Command> GetCommandByPath(Parts& names, const Loader::Commands& commands);
  [[nodiscard]] static Ref<Command> GetCommandByName(const std::string& name, const Loader::Commands& commands);

 private:
  const Ref<Loader>& m_Config;
};

}  // namespace Litr::Config
