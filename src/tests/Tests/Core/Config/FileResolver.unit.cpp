#include <doctest/doctest.h>

#include "Core/Config/FileResolver.hpp"

TEST_SUITE("Config::FileResolver") {
  TEST_CASE("File not found" * doctest::skip()) {
    // @todo: This will find a potential config in home of the executing system.
    Litr::Config::FileResolver config{"/some/path/to/nowhere"};
    CHECK(config.GetStatus() == Litr::Config::FileResolver::Status::NOT_FOUND);
    CHECK(config.GetFilePath() == "");
  }
}
