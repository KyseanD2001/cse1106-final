#include <stdio.h>
#include "fileio.h"

//Saves occupancy events to a CSV file.

void saveEvent(
  const char *eventType,
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
//Write event and occupancy
    fprintf(file, "%s,%d\n", eventType, occupancy);

//close file
    fclose(file);
  }
}

