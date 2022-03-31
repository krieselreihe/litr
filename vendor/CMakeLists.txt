include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
  GIT_TAG b6f4ceaed0a0a24ccf575fab6c56dd50ccf6f1a9 # 8.1.1
)
add_subdirectory(fmt)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
  GIT_TAG eb3220622e73a4889eee355ffa37972b3cac3df5 # 1.9.2
)
add_subdirectory(spdlog)

FetchContent_Declare(
  toml11
  GIT_REPOSITORY "https://github.com/ToruNiina/toml11.git"
  GIT_TAG dcfe39a783a94e8d52c885e5883a6fbb21529019 # 3.7.1
)
add_subdirectory(toml11)

FetchContent_Declare(
  ordered-map
  GIT_REPOSITORY "https://github.com/krieselreihe/ordered-map.git"
  GIT_TAG 7d0e3f22bbf00fda55816ddfa06cb3651ae90edd # task/fix-implicit-sign-conversion
)
add_subdirectory(ordered-map)

FetchContent_Declare(
  doctest
  GIT_REPOSITORY "https://github.com/onqtam/doctest.git"
  GIT_TAG 7b9885133108ae301ddd16e2651320f54cafeba7 # 2.4.8
)
add_subdirectory(doctest)
