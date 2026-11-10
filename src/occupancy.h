#ifndef OCCUPANCY_H
#define OCCUPANCY_H

/*
  Updates room occupancy. This also prevents occupancy from becoming
negative and prevents occupancy from exceeding room capacity.
  */

int updateOccupancy(
  int currentOccupancy,
  int change,
  int maxCapacity
);

#endif
