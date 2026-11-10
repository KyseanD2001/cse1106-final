cmake_minimum_required(VERSION 3.10)

project(room_tracker C)

set(CMAKE_C_STANDARD 11)

add_executable(
  room_tracker

  src/main.c
  src/occupancy.c
  src/display.c
  src/fileio.c
  src/led.c
)

target_link_libraries(
  room_tracker

  pigpio
  rt
  pthread
)
