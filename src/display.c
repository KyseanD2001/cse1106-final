#include <stdio.h>
#include "display.h"

//Display room occupancy, room capacity, and room status.
void displayStatus(
  int occupancy,
  int maxCapacity
)
{ 
  //Display current occupancy
  printf("\nCurrent Occupancy: %d\n", occupancy);

  //Display room capacity
  printf("Maximum Capacity: %d\n", maxCapacity);

  //Check if room is full
  if(occupancy >= maxCapacity)
  {
    printf("Room Status: FULL\n");
  }
  else
  {
  printf("Room Status: AVAILABLE\n");
  }
}
