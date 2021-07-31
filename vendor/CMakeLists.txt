include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
  GIT_TAG d141cdbeb0fb422a3fb7173b285fd38e0d1772dc # 8.0.1
)
add_subdirectory(fmt)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
  GIT_TAG 5b4c4f3f770acbd25400d866f3fc2fdf669d5b7e # 1.9.1
)
add_subdirectory(spdlog)

FetchContent_Declare(
  toml11
  GIT_REPOSITORY "https://github.com/ToruNiina/toml11.git"
  GIT_TAG 647381020ef04b5d41d540ec489eba45e82d90a7 # 3.7.0
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
  GIT_TAG 4d8716f1efc1d14aa736ef52ee727bd4204f4c40 # 2.4.6
)
add_subdirectory(doctest)
