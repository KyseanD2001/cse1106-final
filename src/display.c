#include <stdio.h>
#include "display.h"


void displayStatus(
  int occupancy,
  int maxCapacity
)
{ 
  printf("n\Current Occupancy: %d\n", occupancy);

  printf("Maximum Capacity: %d\n", maxCapacity);

  if(occupancy >= maxCapacity)
  {
    printf("Room Status: FULL\n");
  }
  else
  {
  printf("Room Status: AVAILABLE\n");
  }
}
