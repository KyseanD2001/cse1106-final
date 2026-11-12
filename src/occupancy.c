#include "occupancy.h"

//Updates the current occupancy count

int updateOccupancy(
  int currentOccupancy,
  int change,
  int maxCapacity
)
{
  //Add or subtract students
  currentOccupancy = currentOccupancy + change;

//Prevents occupancy from going below zero
  if(currentOccupancy < 0)
  {
  currentOccupancy = 0;
  }

//Prevent occupancy from exceeding capacity
  if(currentOccupancy > maxCapacity)
  {
  currentOccupancy = maxCapacity;
  }
//Return validated occupancy count
  return currentOccupancy;
}
