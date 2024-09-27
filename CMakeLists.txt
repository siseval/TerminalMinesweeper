cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 23)
project(minesweeper)
add_executable(minesweeper
  minesweeper.c
  menu.c
  )

find_package(Curses REQUIRED)

include_directories(${CURSES_INCLUDE_DIRS})
target_link_libraries(minesweeper ${CURSES_LIBRARY})
