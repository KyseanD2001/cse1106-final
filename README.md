# cse1106-final
Final project for Introduction to Computer Engineering. 

# Smart Study Room Occupancy Tracker

A Raspberry Pi based occupancy tracking system written in C.

The program tracks the number of students in a study room, logs occupancy events to a CSV file, and controls Green and Red LEDs to indicate room availability.

## Features

- Track students entering the room
- Track students leaving the room
- Prevent occupancy from becoming negative
- Prevent occupancy from exceeding room capacity
- Change room capacity while the program is running
- Log occupancy events to a CSV file
- Green LED indicates space is available
- Red LED indicates room is full
- Input validation for menu selections and occupancy updates

## Repository Structure

```text
cse1106-final/
│
├── CMakeLists.txt
│
├── src/
│   ├── main.c
│   ├── occupancy.c
│   ├── occupancy.h
│   ├── display.c
│   ├── display.h
│   ├── fileio.c
│   ├── fileio.h
│   ├── led.c
│   └── led.h
│
├── docs/
│   ├── requirements.md
│   ├── design.md
│   └── change_log.md
│
└── data/
    └── occupancy_log.csv
```

## Hardware Requirements

### Raspberry Pi

Tested on Raspberry Pi OS.

### LEDs

- 1 Green LED
- 1 Red LED
- 2 × 220Ω resistors
- Breadboard
- Jumper wires

### GPIO Connections

Green LED:

```text
GPIO17
   |
220Ω Resistor
   |
Green LED
   |
GND
```

Red LED:

```text
GPIO27
   |
220Ω Resistor
   |
Red LED
   |
GND
```

## Software Requirements

Update system packages:

```bash
sudo apt update
sudo apt upgrade
```

Install build tools:

```bash
sudo apt install build-essential
```

Install CMake:

```bash
sudo apt install cmake
```

Install pigpio:

```bash
sudo apt install pigpio
```

## Clone Repository

```bash
git clone https://github.com/KyseanD2001/cse1106-final.git
```

Enter project directory:

```bash
cd cse1106-final
```

## Build Project

Create build directory:

```bash
mkdir -p build
```

Enter build directory:

```bash
cd build
```

Generate build files:

```bash
cmake ..
```

Compile:

```bash
make
```

Successful compilation should create:

```text
room_tracker
```

## Running the Program

Run with root privileges to access GPIO:

```bash
sudo ./room_tracker
```

## Updating Local Repository

Move to project folder:

```bash
cd ~/cse1106-final
```

Pull latest changes:

```bash
git pull origin main
```

Rebuild:

```bash
cd build
make
```

## Example Usage

```text
1. Student Entered
2. Student Left
3. Change Capacity
4. Display Status
5. Exit
```

Example:

```text
Choice: 1

How many students entered: 5
```

This adds five students to the room occupancy count.

## Troubleshooting

### GPIO Not Working

Make sure pigpio is installed:

```bash
sudo apt install pigpio
```

Run the program with:

```bash
sudo ./room_tracker
```

### Build Errors

Delete build directory and rebuild:

```bash
rm -rf build

mkdir build
cd build

cmake ..
make
```

### CSV File Not Updating

Verify that the data directory exists:

```bash
mkdir -p data
```

## Authors

- Kysean Dixon
- Daniel

## License

Educational use only.
