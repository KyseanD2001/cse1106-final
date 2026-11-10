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

  int numPeople;

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
    if(scanf("%d", &choice) !=1)
      {
        printf("\nInvalid input. Please enter a menu number.\n");
        while(getchar() != '\n');

        continue;
    }

    if(choice == 1)
    {
      do
      {
        //Students enterd
        printf("\nHow many students entered: ");

        if(scanf("%d", &numPeople) != 1)
        {
          printf("\nInvalid input. Please enter a number.\n");

          while(getchar()!='\n');
          numPeople = -1;
        }
        else if(numPeople <= 0)
        {
          printf("\nPlease enter a positive number.\n"); 
        }
          
      } while(numPeople <= 0);

      
      occupancy = updateOccupancy(occupancy,numPeople, maxCapacity);
      //Log Event
      saveEvent("ENTER",occupancy);

      displayStatus(occupancy,maxCapacity);
    }
  
    else if(choice == 2)
    {
      do
      {
        //Student left room
        printf("\nHow many students left: ");

        if(scanf("%d", &numPeople) != 1)
        {
          printf("\nInvalid input. Please enter a number.\n");

          while(getchar()!='\n');
          numPeople = -1;
        }
        else if(numPeople <= 0)
        {
          printf("\nPlease enter a positive number.\n"); 
        }
          
      }while(numPeople <= 0);
      
      occupancy = updateOccupancy(occupancy,-numPeople, maxCapacity);
//Log Event
      saveEvent("EXIT", occupancy);

      displayStatus(occupancy,maxCapacity);
    }
    else if(choice == 3)
    {
//Change room capacity
    do
    {
      printf("\nEnter New Capacity: ");
      if(scanf("%d", &maxCapacity) != 1)
      {
          printf("\nInvalid input. Please enter a number.\n");

          while(getchar() != '\n');

          maxCapacity = -1;
      }
      else if(maxCapacity <= 0)
      {
        printf("\nCapacity must be greater than 0.\n");
      }
    } while(maxCapacity <= 0);

        
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


