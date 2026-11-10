#include <stdio.h>

#include "occupancy.h"
#include "display.h"
#include "fileio.h"
#include "led.h"

//Smart Study Room Occupancy Tracker

int main(void)
{
  //Current number of students
  int occupancy = 0
