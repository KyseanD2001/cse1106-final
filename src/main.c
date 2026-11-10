#include <stdio.h>

#include "occupancy.h"
#include "display.h"
#include "fileio.h"
#include "led.h"

//Smart Study Room Occupancy Tracker

int main(void)
{
  //Current number of students
  int occupancy = 0;

//Maximum room capacity; can be changed if TA asks.
  int maxCapacity = 5

//User menu selection
  int choice = 0

//Initialize GPIO pins
setupLED();

//Run until user chooses Exit
while(choice != 5)
  {
    printf("\n SMART STUDY ROOM TRACKER BY KYSEAN AND DANIEL\n");
    printf("1. Student Entered\n");
    printf("2. Student Left\n");
    printf("3. Change Capacity\n");
    printf("4. Display Status\n");
    printf("5. Exit\n");

