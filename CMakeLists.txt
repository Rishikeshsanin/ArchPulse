cmake_minimum_required(VERSION 3.20)
project(ArchPulse VERSION 2.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
option(ARCHPULSE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
add_library(archpulse_core
  src/cache.cpp
  src/pipeline.cpp
  src/tlb.cpp
  src/memory_hierarchy.cpp
  src/coherence.cpp
  src/thread_pool.cpp
  src/benchmark.cpp
  src/metrics.cpp
)
target_include_directories(archpulse_core PUBLIC include)
if(MSVC)
  target_compile_options(archpulse_core PRIVATE /W4)
  if(ARCHPULSE_WARNINGS_AS_ERRORS)
    target_compile_options(archpulse_core PRIVATE /WX)
  endif()
else()
  target_compile_options(archpulse_core PRIVATE -Wall -Wextra -Wpedantic)
  if(ARCHPULSE_WARNINGS_AS_ERRORS)
    target_compile_options(archpulse_core PRIVATE -Werror)
  endif()
endif()
find_package(Threads REQUIRED)
target_link_libraries(archpulse_core PUBLIC Threads::Threads)
add_executable(archpulse src/main.cpp)
target_link_libraries(archpulse PRIVATE archpulse_core)
add_executable(archpulse_coherence src/coherence_cli.cpp)
target_link_libraries(archpulse_coherence PRIVATE archpulse_core)
include(CTest)
if(BUILD_TESTING)
  add_executable(archpulse_tests tests/test_main.cpp)
  target_link_libraries(archpulse_tests PRIVATE archpulse_core)
  add_test(NAME archpulse_tests COMMAND archpulse_tests)
endif()
