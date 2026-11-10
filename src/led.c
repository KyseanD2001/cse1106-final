#include <pigpio.h>
#include "led.h"

//GPIO pin connect to Green LED
#define GREEN_LED_PIN 17

//GPIO pin connected to Red LED
#define RED_LED_PIN 27

// Initialize pigio and configure LED pins.

void setupLED(void)
{
  gpioInitialise();

//Configure Green LED pin as output
  gpioSetMode(
    GREEN_LED_PIN,
    PI_OUTPUT
  );
//Configure Red LED pin as output
  gpioSetMode(
    RED_LED_PIN,
    PI_OUTPUT
  );
}

//update the LED status
void updateLED(
  int occupancy,
  int maxCapacity
)
{
  //Check if room is full
  if(occupancy >= maxCapacity)
  {
//Turn Green LED off
    gpioWrite(
      GREEN_LED_PIN, 0
      );
//Turn Red LED on
    gpioWrite(
      RED_LED_PIN, 1
      );
  }
  else
  {
//Turn Green LED on
    gpioWrite(
      GREEN_LED_PIN, 1
      );
//Turn Red LED off
    gpioWrite(
      RED_LED_PIN, 0
    );
  }
}
//Turn off LEDs and terminate pigpio
void cleanupLED(void)
{
  //Turn Green LED off
  gpioWrite(
    GREEN_LED_PIN, 0
  );
//Turn RED LED off
  gpioWrite(
    RED_LED_PIN, 0
  );
//Shut down pigpio
  gpioTerminate();
}



    
