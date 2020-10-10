#include <doctest/doctest.h>

#include "Core/Log.hpp"
#include "Core/ConfigFile.hpp"

TEST_SUITE("ConfigFile") {
  TEST_CASE("File not found") {
    Litr::Log::Init();
    Litr::ConfigFile config{"/some/path/to/nowhere"};
    CHECK(config.GetStatus() == Litr::ConfigFile::Status::NOT_FOUND);
    CHECK(config.GetFilePath() == "");
  }

  // @todo: Tests cases for finding a file in different scenarios,
  // made possible by mocking.
}
