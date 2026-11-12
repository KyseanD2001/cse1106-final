#ifndef FILEIO_H
#define FILEIO_H

//Saves occupancy events to the log file
void saveEvent(
  const char *eventType,
  int numPeople,
  int occupancy
);

#endif

