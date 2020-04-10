cmake_minimum_required(
  VERSION
    3.15
)

project(
  template_programming_workshop
  VERSION
    0.0
  DESCRIPTION
    "workshop for learning some template programming patterns and idioms"
)

enable_testing()

set(
  CMAKE_EXPORT_COMPILE_COMMANDS
  ON
)

add_subdirectory(
  src
)
