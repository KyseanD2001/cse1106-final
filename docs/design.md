# Design

## Overview

This project is a Raspberry Pi based Study Room Occupancy Tracker.

The program keeps track of how many students are in a room, displays the current status, saves events to a CSV file, and uses LEDs to indicate when the room is available or full.

## Team Responsibilities

### Kysean

Files:

occupancy.c
occupancy.h
led.c
led.h

Responsibilities:

- Occupancy validation
- Capacity enforcement
- GPIO setup
- LED control

### Daniel

Files:

display.c
display.h
fileio.c
fileio.h

Responsibilities:

- Display formatting
- CSV logging
- File management

  ###########################################################

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
- Gets user input using scanf()
- Controls overall program flow

occupancy.c
- Updates occupancy count
- Prevents invalid occupancy values

display.c
- Prints occupancy information
- Prints room status

fileio.c
- Output File to data/occupancy_log.csv

led.c
- Controls Green and Red LEDs using pigpio

## GPIO Assignments
GREEN_LED_PIN = 17
RED_LED_PIN = 27

## Approved Function Names
The following function names are part of the project's public interface and will not be changed without team agreement.

### Occupancy Module
int updateOccupancy(
    int currentOccupancy,
    int change,
    int maxCapacity
);

Purpose:
  - Add students
  - Remove students
  - Prevent negative occupancy
  - Prevent occupancy above room capacity

### Display Module
void displayStatus(
    int occupancy,
    int maxCapacity
);

Purpose:
- Display occupancy
- Display room status

### File Module

void saveEvent(
      const char *eventType,
      int occupancy
);

Purpose:
- Save events to CSV


## LED Module
void setupLED(void);

Purpose:
- Initialize pigpio
- Configure GPIO pins

  

void updateLED(
      int occupancy,
      int maxCapacity
);

Purpose:

- Update Green and Red LED status

void cleanupLED(void);

Purpose:
- Turn off LEDS
- Stop pigpio safely

## Variables Used in main.c

 These are variables that should be used in main.c
 
int occupancy;
int maxCapacity;
int choice;


Purpose:

- occupancy = current number of students in the room
- maxCapacity = maximum room capacity
- choice = user menu selection


## Program Flow
1. Start Program

2. Initialize LEDs

3. Display Menu

4. Get User Input

5. Process User Choice

   Option 1:
   Student Entered
   -> Update Occupancy
   -> Save Event

   Option 2:
   Student Left
   -> Update Occupancy
   -> Save Event

   Option 3:
   Change Capacity

   Option 4:
   Exit Program

6. Display Status

7. Update LEDs

8. Return To Menu

9. Cleanup LEDs

10. End Program


##Build

gcc src/main.c
src/occupancy.c
src/display.c
src/fileio.c
src/led.c
-lpigpio -lrt -pthread
-o room_tracker
###RUN
sudo ./room_tracker
