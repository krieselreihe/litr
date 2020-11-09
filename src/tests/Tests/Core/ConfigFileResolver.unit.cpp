#include <doctest/doctest.h>

#include "Core/Config/ConfigFileResolver.hpp"
#include "Core/Log.hpp"

TEST_SUITE("ConfigFileResolver") {
  TEST_CASE("File not found") {
    Litr::Log::Init();
    Litr::ConfigFileResolver config{"/some/path/to/nowhere"};
    CHECK(config.GetStatus() == Litr::ConfigFileResolver::Status::NOT_FOUND);
    CHECK(config.GetFilePath() == "");
  }

  // @todo: Test cases for finding a file in different scenarios,
  // made possible by mocking.
  // https://github.com/krieselreihe/litr/issues/12
}
