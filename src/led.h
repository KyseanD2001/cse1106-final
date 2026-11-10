#ifndef LED_H
#define LED_H

void setupLED(void);

void updateLED(
  int occupancy,
  int maxCapacity
);

void cleanupLED(void);

#endif
