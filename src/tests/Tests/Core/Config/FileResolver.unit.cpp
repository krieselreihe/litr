#include <doctest/doctest.h>

#include "Core/Config/FileResolver.hpp"

TEST_SUITE("Config::FileResolver") {
  TEST_CASE("File not found") {
    Litr::Config::FileResolver config{"/some/path/to/nowhere"};
    CHECK(config.GetStatus() == Litr::Config::FileResolver::Status::NOT_FOUND);
    CHECK(config.GetFilePath() == "");
  }

  // @todo: Test cases for finding a file in different scenarios,
  // made possible by mocking.
  // https://github.com/krieselreihe/litr/issues/12
  // @hint: Could be done by using FakeIt and making FileSystem a singleton. But then
  // methods need to be public. So there would be a tradeoff.
}
