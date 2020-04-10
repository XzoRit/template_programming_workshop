set(
  BOOST_ROOT
  $ENV{BOOST_ROOT}
  )
set(
  BOOST_LIBRARYDIR
  "${BOOST_ROOT}/stage/lib"
  )
set(
  Boost_NO_SYSTEM_PATHS
  ON
  )

find_package(
  Boost
  REQUIRED
  COMPONENTS
  )

if(Boost_FOUND)
  set_target_properties(
    Boost::boost
    PROPERTIES
    IMPORTED_GLOBAL
    true
    )
endif()

add_executable(
  workshop
  )

target_sources(
  workshop
  PRIVATE
  main.cpp
  )

target_link_libraries(
  workshop
  PRIVATE
  Boost::boost
  )

add_test(
  NAME
  workshop::exe
  COMMAND
  workshop
  )
