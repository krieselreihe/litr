/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core/StringUtils.hpp"

#include <doctest/doctest.h>

#include <deque>
#include <string>
#include <vector>

TEST_SUITE("StringUtils") {
  TEST_CASE("trim_left") {
    SUBCASE("Does trim only left side of string") {
      CHECK_EQ(Litr::StringUtils::trim_left("  Bob  ", ' '), "Bob  ");
    }

    SUBCASE("Trims nothing if nothing found") {
      CHECK_EQ(Litr::StringUtils::trim_left("..Bob..", ' '), "..Bob..");
    }
  }

  TEST_CASE("trim_right") {
    SUBCASE("Does trim only right side of string") {
      CHECK_EQ(Litr::StringUtils::trim_right("  Bob  ", ' '), "  Bob");
    }

    SUBCASE("Trims nothing if nothing found") {
      CHECK_EQ(Litr::StringUtils::trim_right("..Bob..", ' '), "..Bob..");
    }
  }

  TEST_CASE("trim") {
    SUBCASE("Trims both sides of string") {
      CHECK_EQ(Litr::StringUtils::trim("....Bob..", '.'), "Bob");
    }
  }

  TEST_CASE("utf8_length") {
    SUBCASE("Gives the correct length for wide strings") {
      CHECK_EQ(Litr::StringUtils::utf8_length("├"), 1);
    }
  }

  TEST_CASE("split_into") {
    SUBCASE("Multiple items into vector") {
      const std::string input{"This\nIs\nA\nTest"};
      std::vector<std::string> output{};
      Litr::StringUtils::split_into(input, '\n', output);
      CHECK_EQ(output.size(), 4);
    }

    SUBCASE("One item into vector") {
      const std::string input{"This Is A Test"};
      std::vector<std::string> output{};
      Litr::StringUtils::split_into(input, '\n', output);
      CHECK_EQ(output.size(), 1);
    }

    SUBCASE("Multiple items into deque") {
      const std::string input{"path.to.somewhere"};
      std::deque<std::string> output{};
      Litr::StringUtils::split_into(input, '.', output);
      CHECK_EQ(output.size(), 3);
    }

    SUBCASE("One item into deque") {
      const std::string input{"path.to.somewhere"};
      std::deque<std::string> output{};
      Litr::StringUtils::split_into(input, ' ', output);
      CHECK_EQ(output.size(), 1);
    }
  }

  TEST_CASE("deduplicate") {
    SUBCASE("Removes duplicates") {
      std::vector<std::string> items{{"Cobra", "Python", "DBX", "Python", "DBX", "DBX", "Clipper"}};
      Litr::StringUtils::deduplicate(items);
      CHECK_EQ(items.size(), 4);
    }

    SUBCASE("Does nothing if no duplicates found") {
      std::vector<std::string> items{{"Cobra", "Python", "DBX", "Clipper"}};
      Litr::StringUtils::deduplicate(items);
      CHECK_EQ(items.size(), 4);
    }
  }

  TEST_CASE("replace") {
    SUBCASE("replace part of string") {
      const std::string source{"This is an example"};
      const std::string from{"is an"};
      const std::string to{"is not an"};
      CHECK_EQ(Litr::StringUtils::replace(source, from, to), "This is not an example");
    }

    SUBCASE("Replaces nothing if nothing found") {
      const std::string source{"This is an example"};
      const std::string from{"Rand"};
      const std::string to{"Perrin"};
      CHECK_EQ(Litr::StringUtils::replace(source, from, to), "This is an example");
    }
  }

  TEST_CASE("find_position_by_whole_word") {
    SUBCASE("returns string length if less than max search length") {
      const std::string value{"This is an example"};
      const size_t max_length{20};
      CHECK_EQ(Litr::StringUtils::find_position_by_whole_word(value, max_length), 18);
    }

    SUBCASE("returns 0 if there is no whole word") {
      const std::string value{"Thisisanexample"};
      const size_t max_length{10};
      CHECK_EQ(Litr::StringUtils::find_position_by_whole_word(value, max_length), 0);
    }

    SUBCASE("returns whole word position") {
      const std::string value{"This is an example"};
      const size_t max_length{15};
      CHECK_EQ(Litr::StringUtils::find_position_by_whole_word(value, max_length), 10);
    }
  }

  TEST_CASE("split_by_whole_word") {
    SUBCASE("returns one line if string length is less than max length") {
      const std::string value{"This is an example sentence to split."};
      const size_t max_length{38};
      const auto lines{Litr::StringUtils::split_by_whole_word(value, max_length)};
      CHECK_EQ(lines.size(), 1);
      CHECK_EQ(lines[0], value);
    }

    SUBCASE("splits sentence into parts by whole words") {
      const std::string value{"This is an example sentence to split."};
      const size_t max_length{25};
      const auto lines{Litr::StringUtils::split_by_whole_word(value, max_length)};
      CHECK_EQ(lines.size(), 2);
      CHECK_EQ(lines[0], "This is an example");
      CHECK_EQ(lines[1], "sentence to split.");
    }

    SUBCASE("splits sentence into many parts by whole words") {
      const std::string value{"This is an example sentence to split into many different parts."};
      const size_t max_length{20};
      const auto lines{Litr::StringUtils::split_by_whole_word(value, max_length)};
      CHECK_EQ(lines.size(), 4);
      CHECK_EQ(lines[0], "This is an example");
      CHECK_EQ(lines[1], "sentence to split");
      CHECK_EQ(lines[2], "into many different");
      CHECK_EQ(lines[3], "parts.");
    }
  }
}
