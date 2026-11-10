# Design

## Overview

This project is a Raspberry Pi based Study Room Occupancy Tracker.

The program keeps track of how many students are in a room, displays the current status, saves events to a CSV file, and uses LEDs to indicate when the room is available or full.

## File Structure

src/
├── main.c
├── occupancy.c
├── occupancy.h
├── display.c
├── display.h
├── fileio.c
├── fileio.h
├── led.c
└── led.h

## Module Responsibilities

main.c
- Handles menu navigation
- Calls functions from other modules

occupancy.c
- Updates occupancy count
- Prevents invalid occupancy values

display.c
- Prints occupancy information
- Prints room status

fileio.c
- Saves events to occupancy_log.csv

led.c
- Controls Green and Red LEDs using pigpio
