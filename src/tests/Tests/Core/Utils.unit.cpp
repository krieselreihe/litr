/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core/Utils.hpp"

#include <doctest/doctest.h>

#include <deque>
#include <string>
#include <vector>

TEST_SUITE("Utils") {
  TEST_CASE("trim_left") {
    SUBCASE("Does trim only left side of string") {
      CHECK_EQ(Litr::Utils::trim_left("  Bob  ", ' '), "Bob  ");
    }

    SUBCASE("Trims nothing if nothing found") {
      CHECK_EQ(Litr::Utils::trim_left("..Bob..", ' '), "..Bob..");
    }
  }

  TEST_CASE("trim_right") {
    SUBCASE("Does trim only right side of string") {
      CHECK_EQ(Litr::Utils::trim_right("  Bob  ", ' '), "  Bob");
    }

    SUBCASE("Trims nothing if nothing found") {
      CHECK_EQ(Litr::Utils::trim_right("..Bob..", ' '), "..Bob..");
    }
  }

  TEST_CASE("trim") {
    SUBCASE("Trims both sides of string") {
      CHECK_EQ(Litr::Utils::trim("....Bob..", '.'), "Bob");
    }
  }

  TEST_CASE("split_into") {
    SUBCASE("Multiple items into vector") {
      const std::string input{"This\nIs\nA\nTest"};
      std::vector<std::string> output{};
      Litr::Utils::split_into(input, '\n', output);
      CHECK_EQ(output.size(), 4);
    }

    SUBCASE("One item into vector") {
      const std::string input{"This Is A Test"};
      std::vector<std::string> output{};
      Litr::Utils::split_into(input, '\n', output);
      CHECK_EQ(output.size(), 1);
    }

    SUBCASE("Multiple items into deque") {
      const std::string input{"path.to.somewhere"};
      std::deque<std::string> output{};
      Litr::Utils::split_into(input, '.', output);
      CHECK_EQ(output.size(), 3);
    }

    SUBCASE("One item into deque") {
      const std::string input{"path.to.somewhere"};
      std::deque<std::string> output{};
      Litr::Utils::split_into(input, ' ', output);
      CHECK_EQ(output.size(), 1);
    }
  }

  TEST_CASE("deduplicate") {
    SUBCASE("Removes duplicates") {
      std::vector<std::string> items{{"Cobra", "Python", "DBX", "Python", "DBX", "DBX", "Clipper"}};
      Litr::Utils::deduplicate(items);
      CHECK_EQ(items.size(), 4);
    }

    SUBCASE("Does nothing if no duplicates found") {
      std::vector<std::string> items{{"Cobra", "Python", "DBX", "Clipper"}};
      Litr::Utils::deduplicate(items);
      CHECK_EQ(items.size(), 4);
    }
  }

  TEST_CASE("replace") {
    SUBCASE("replace part of string") {
      const std::string source{"This is an example"};
      const std::string from{"is an"};
      const std::string to{"is not an"};
      CHECK_EQ(Litr::Utils::replace(source, from, to), "This is not an example");
    }

    SUBCASE("Replaces nothing if nothing found") {
      const std::string source{"This is an example"};
      const std::string from{"Rand"};
      const std::string to{"Perrin"};
      CHECK_EQ(Litr::Utils::replace(source, from, to), "This is an example");
    }
  }
}
