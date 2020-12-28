#include "Parser.hpp"

#include "Core/CLI/Scanner.hpp"

namespace Litr {

Parser::Parser(const std::string& source) {
  Scanner scanner{source.c_str()};

  // @todo: Dummy parsing, will be replaced.
  int line{-1};
  for (;;) {
    Scanner::Token token{scanner.ScanToken()};
    if (token.Line != line) {
      printf("%4d ", token.Line);
      line = token.Line;
    } else {
      printf("   | ");
    }
    printf("%2d '%.*s'\n", token.Type, token.Length, token.Start);

    if (token.Type == Scanner::TokenType::EOS) break;
  }
}

}  // namespace Litr
