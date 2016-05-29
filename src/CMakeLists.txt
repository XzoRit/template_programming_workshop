cmake_minimum_required(VERSION 3.5)
project("template_programming_workshop")

include_directories(SYSTEM "/opt/local/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(template_programming_workshop
  main.cpp)
set_property(TARGET template_programming_workshop PROPERTY CXX_STANDARD 14)
set_property(TARGET template_programming_workshop PROPERTY CXX_STANDARD_REQUIRED ON)
