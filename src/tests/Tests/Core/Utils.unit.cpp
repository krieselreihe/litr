/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include <doctest/doctest.h>

#include <vector>
#include <deque>
#include <string>

#include "Core/Utils.hpp"

TEST_SUITE("Utils") {
  TEST_CASE("TrimLeft") {
    SUBCASE("Does trim only left side of string") {
      CHECK_EQ(Litr::Utils::TrimLeft("  Bob  ", ' '), "Bob  ");
    }

    SUBCASE("Trims nothing if nothing found") {
      CHECK_EQ(Litr::Utils::TrimLeft("..Bob..", ' '), "..Bob..");
    }
  }

  TEST_CASE("TrimRight") {
    SUBCASE("Does trim only right side of string") {
      CHECK_EQ(Litr::Utils::TrimRight("  Bob  ", ' '), "  Bob");
    }

    SUBCASE("Trims nothing if nothing found") {
      CHECK_EQ(Litr::Utils::TrimRight("..Bob..", ' '), "..Bob..");
    }
  }

  TEST_CASE("Trim") {
    SUBCASE("Trims both sides of string") {
      CHECK_EQ(Litr::Utils::Trim("....Bob..", '.'), "Bob");
    }
  }

  TEST_CASE("SplitInto") {
    SUBCASE("Multiple items into vector") {
      const std::string input{"This\nIs\nA\nTest"};
      std::vector<std::string> output{};
      Litr::Utils::SplitInto(input, '\n', output);
      CHECK_EQ(output.size(), 4);
    }

    SUBCASE("One item into vector") {
      const std::string input{"This Is A Test"};
      std::vector<std::string> output{};
      Litr::Utils::SplitInto(input, '\n', output);
      CHECK_EQ(output.size(), 1);
    }

    SUBCASE("Multiple items into deque") {
      const std::string input{"path.to.somewhere"};
      std::deque<std::string> output{};
      Litr::Utils::SplitInto(input, '.', output);
      CHECK_EQ(output.size(), 3);
    }

    SUBCASE("One item into deque") {
      const std::string input{"path.to.somewhere"};
      std::deque<std::string> output{};
      Litr::Utils::SplitInto(input, ' ', output);
      CHECK_EQ(output.size(), 1);
    }
  }

  TEST_CASE("Deduplicate") {
    SUBCASE("Removes duplicates") {
      std::vector<std::string> items{{
          "Cobra", "Python", "DBX", "Python", "DBX", "DBX", "Clipper"
      }};
      Litr::Utils::Deduplicate(items);
      CHECK_EQ(items.size(), 4);
    }

    SUBCASE("Does nothing if no duplicates found") {
      std::vector<std::string> items{{
          "Cobra", "Python", "DBX", "Clipper"
      }};
      Litr::Utils::Deduplicate(items);
      CHECK_EQ(items.size(), 4);
    }
  }

  TEST_CASE("Replace") {
    SUBCASE("Replace part of string") {
      const std::string source{"This is an example"};
      const std::string from{"is an"};
      const std::string to{"is not an"};
      CHECK_EQ(Litr::Utils::Replace(source, from, to), "This is not an example");
    }

    SUBCASE("Replaces nothing if nothing found") {
      const std::string source{"This is an example"};
      const std::string from{"Rand"};
      const std::string to{"Perrin"};
      CHECK_EQ(Litr::Utils::Replace(source, from, to), "This is an example");
    }
  }
}
