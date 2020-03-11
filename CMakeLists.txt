cmake_minimum_required(VERSION 3.15)
project(dpll)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -Wall")

add_executable(CHRONOLOGICAL_BACKTRACK_DPLL main2.cpp DimacsParser.h common.h CHRONOLOGICAL_BACKTRACK_DPLL.cpp CHRONOLOGICAL_BACKTRACK_DPLL.h)
add_executable(dpll main.cpp DimacsParser.h DPLL.cpp DPLL.h)
add_executable(compare compare.cpp DimacsParser.h DPLL.h DPLL.cpp common.h CHRONOLOGICAL_BACKTRACK_DPLL.cpp CHRONOLOGICAL_BACKTRACK_DPLL.h)