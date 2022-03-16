/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core/Config/FileResolver.hpp"

#include <doctest/doctest.h>

TEST_SUITE("Config::FileResolver") {
  TEST_CASE("File not found" * doctest::skip()) {
    // @todo: This will find a potential config in home of the executing system.
    // So mocking would be solution, maybe.
    litr::config::FileResolver config{"/some/path/to/nowhere"};
    CHECK_EQ(config.get_status(), litr::config::FileResolver::Status::NOT_FOUND);
    CHECK_EQ(config.get_file_path(), "");
  }
}
