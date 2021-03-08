#include "Handler.hpp"

#include "Core/Debug/Instrumentor.hpp"

namespace Litr::Error {

void Handler::Push(const BaseError& err) {
  LITR_PROFILE_FUNCTION();

  Get().m_Errors.push_back(err);
}

void Handler::Flush() {
  LITR_PROFILE_FUNCTION();

  Get().m_Errors.clear();
}

std::vector<BaseError> Handler::GetErrors() {
  LITR_PROFILE_FUNCTION();

  return Get().m_Errors;
}

bool Handler::HasErrors() {
  LITR_PROFILE_FUNCTION();

  return !Get().m_Errors.empty();
}

std::string Handler::GetTypeDescription(const BaseError& err) {
  LITR_PROFILE_FUNCTION();

  switch (err.Type) {
    case ErrorType::RESERVED_PARAM:
      return "Parameter name is reserved!";
    case ErrorType::MALFORMED_FILE:
      return "Invalid file format!";
    case ErrorType::MALFORMED_COMMAND:
      return "Command format is wrong!";
    case ErrorType::MALFORMED_PARAM:
      return "Parameter format is wrong!";
    case ErrorType::MALFORMED_SCRIPT:
      return "Command script is wrong!";
    case ErrorType::UNKNOWN_COMMAND_PROPERTY:
      return "Command property does not exist!";
    case ErrorType::UNKNOWN_PARAM_VALUE:
      return "Parameter value is not known!";
    case ErrorType::PARSER_ERROR:
      return "Problem parsing command line arguments!";
    case ErrorType::COMMAND_NOT_FOUND:
      return "Command not found!";
    case ErrorType::EXECUTION_FAILURE:
      return "Problem executing a command!";
  }
}

}  // namespace Litr::Error
