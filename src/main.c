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
  int maxCapacity = 5;

//User menu selection
  int choice = 0;

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

    printf("\nEnter Choice: ");
    scanf("%d", &choice);

    if(choice == 1)
    {
    //Student entered room
      occupancy = updateOccupancy(occupancy,1, maxCapacity);
      //Log Event
      saveEvent("ENTER",occupancy);

      displayStatus(occupancy,maxCapacity);
    }
    else if(choice == 2)
    {
//Student left room
      occupancy = updateOccupancy(occupancy, -1, maxCapacity);
//Log Event
      saveEvent("EXIT", occupancy);

      displayStatus(occupancy,maxCapacity);
    }
    else if(choice == 3)
    {
//Change room capacity
      printf("\nEnter New Capacity: ");
      scanf("%d", &maxCapacity);
//Validate occupancy does not exceed new capacity
      occupancy = updateOccupancy (occupancy, 0, maxCapacity);

      displayStatus(occupancy,maxCapacity);
    }

    else if(choice == 4)
    {
//Display room information
      displayStatus(occupancy,maxCapacity);
    }
    else if(choice == 5)
    {
      printf("\nExiting Program...\n");
    }
    else
    {
      printf("\nInvalid Choice.\n");
    }

//Update LEDs every loop
    updateLED(occupancy,maxCapacity);
    }
// Turn off LEDS and release GPIO
  cleanupLED();

  return 0;
  }


