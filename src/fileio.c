#include <stdio.h>
#include "fileio.h"

//Saves occupancy events to a CSV file.

void saveEvent(
  const char *eventType,
  int numPeople,
  int occupancy
)
{
  //Create file pointer
  FILE *file;

//Open CSV file in append mode
  file = fopen("data/occupancy_log.csv", "a");

//Verify opened file successfully
  if(file != NULL)
  {
//Write event, occupancy, and max capacity
    fprintf(file, "%s,%d,%d\n", eventType, occupancy, numPeople);

//close file
    fclose(file);
  }
}

