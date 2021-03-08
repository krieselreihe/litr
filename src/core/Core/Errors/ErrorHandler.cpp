#include "ErrorHandler.hpp"

#include "Core/Debug/Instrumentor.hpp"

namespace Litr {

void ErrorHandler::Push(const Error& err) {
  LITR_PROFILE_FUNCTION();

  Get().m_Errors.push_back(err);
}

void ErrorHandler::Flush() {
  LITR_PROFILE_FUNCTION();

  Get().m_Errors.clear();
}

std::vector<Error> ErrorHandler::GetErrors() {
  LITR_PROFILE_FUNCTION();

  return Get().m_Errors;
}

bool ErrorHandler::HasErrors() {
  LITR_PROFILE_FUNCTION();

  return !Get().m_Errors.empty();
}

std::string ErrorHandler::GetTypeDescription(const Error& err) {
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

}  // namespace Litr
