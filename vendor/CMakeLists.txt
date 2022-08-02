include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
  GIT_TAG 9.0.0
)
add_subdirectory(fmt)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
  GIT_TAG v1.10.0
)
add_subdirectory(spdlog)

FetchContent_Declare(
  toml11
  GIT_REPOSITORY "https://github.com/ToruNiina/toml11.git"
  GIT_TAG v3.7.1
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
  GIT_TAG v2.4.9
)
add_subdirectory(doctest)
