// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI
// By: Roger Tsang
// Version: Alpha

#include <stdlib.h>
#include <stdio.h>
//#include <time.h>
#include "Game.h"
#include "Map.h"
#include "DracView.h"
#include "Queue.h"

//#define DEBUG

#define SIZE_TAUNT MESSAGE_SIZE
#define NUM_HUNTER 4

//UPDATE: ALPHA
static LocationID drac_nextLocation(DracView gameState, LocationID destination);
static LocationID drac_idealPlace(DracView gameState);
//static LocationID drac_randomPlace(DracView gameState);
static LocationID drac_autoNextMove(DracView gameState);
static int drac_duplicated(LocationID trail[TRAIL_SIZE]);
static int drac_isLegalMove(DracView gameState, LocationID destination);
static void map_howManyStep(DracView gameState, int steps[NUM_MAP_LOCATIONS]);

//UPDATE: SHIT THAT BOAT AND SEA, FREAKING CONFUSING!

void decideDraculaMove(DracView gameState)
{
   //srand(time(NULL));
   //0   Step: List out frequently used variable;
   Round game_round = giveMeTheRound(gameState);

   //1st Step: Initilize Variable
   LocationID placeToGo;
   
   //2nd Step: If it is round-0
   if (game_round == 0) {
      placeToGo = drac_idealPlace(gameState);
   } else {
      placeToGo = drac_autoNextMove(gameState);
   }

   // Replace the line below by something better
   registerBestPlay(idToabbrev(placeToGo), "10/17/2014 4:39PM");
   #ifdef DEBUG
   LocationID trail[TRAIL_SIZE];
   giveMeTheTrail(gameState, PLAYER_DRACULA, trail);
   int i;
   for (i = 0; i < TRAIL_SIZE; i++) {
      printf("[%d]\t",trail[i]);
   }
   #endif
   //free used memory
}

static LocationID drac_nextLocation(DracView gameState, LocationID destination) {
   int myLocation = whereIs(gameState, PLAYER_DRACULA);
   int nextLocation;

   LocationID *path = malloc(sizeof(LocationID) * NUM_MAP_LOCATIONS);
   TransportID *tran = malloc(sizeof(TransportID) * NUM_MAP_LOCATIONS);
   shortestPath(path, myLocation, destination, FALSE, tran);
   nextLocation = path[0];
   free(path);
   free(tran);
   return nextLocation;
}

static LocationID drac_idealPlace(DracView gameState) {
   int steps[NUM_MAP_LOCATIONS] = {0};
   int i = 0, max = 0;
   LocationID idealPlace = UNKNOWN_LOCATION;
   map_howManyStep(gameState, steps);
   for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
      if (steps[i] > max) {
         max = steps[i];
         idealPlace = i;
      }
   }
   return idealPlace;
}

/*static LocationID drac_randomPlace(DracView gameState) {
   LocationID *places;
   int numPlaces, numRandom;
   places = whereCanIgo(gameState, &numPlaces, TRUE, FALSE);
   numRandom = rand() % numPlaces;
   return places[numRandom];
}
*/
static LocationID drac_autoNextMove(DracView gameState) {
   LocationID myLocation = whereIs(gameState, PLAYER_DRACULA);
   LocationID idealDes = drac_nextLocation(gameState, drac_idealPlace(gameState));
   LocationID finalDes;
   LocationID *placeICanReach = malloc(sizeof(LocationID) * NUM_MAP_LOCATIONS);
   int numLocations, i, isLegal = FALSE;
   #ifdef DEBUG
   printf("idealPlace = %d\n", idealDes);
   printf("ideal_legal? = %d\n", drac_isLegalMove(gameState, idealDes));
   #endif
   if (drac_isLegalMove(gameState, idealDes)) {
      finalDes = idealDes;
   } else {
      //I don't want to go to the sea for some reason!
      Drac_whereCanIGo(placeICanReach, &numLocations, myLocation, TRUE, FALSE);
      for (i = 0; i < numLocations && isLegal == FALSE; i++) {
         finalDes = placeICanReach[i];
         isLegal = drac_isLegalMove(gameState, finalDes);
      }
      //But if i have to go, then i go for it!
      if (isLegal == FALSE) {
         Drac_whereCanIGo(placeICanReach, &numLocations, myLocation, TRUE, TRUE);
         for (i = 0; i < numLocations && isLegal == FALSE; i++) {
            finalDes = placeICanReach[i];
            printf("%d -> ", finalDes);
            isLegal = drac_isLegalMove(gameState, finalDes);
         }
      }
   #ifdef DEBUG
   printf("finalPlace = %d\n", finalDes);
   printf("final_legal? = %d\n", drac_isLegalMove(gameState, finalDes));
   #endif
   }
   free(placeICanReach);
   return finalDes;
}

static int drac_duplicated(LocationID trail[TRAIL_SIZE]) {
   int i, j, duplicate = 0;
   for (i = 0; i < TRAIL_SIZE-1; i++) {
      for (j = i + 1; j < TRAIL_SIZE; j++) {
         if (trail[i] != -1 && trail[j] != -1 && trail[i] == trail[j]) {
            duplicate++;
         }
      }
   }
   #ifdef DEBUG
   printf("duplicate = %d\n", duplicate);
   #endif
   return duplicate;
}

static int drac_isLegalMove(DracView gameState, LocationID destination) {
   LocationID trail[TRAIL_SIZE];
   int i, isLegal;
   giveMeTheTrail(gameState, PLAYER_DRACULA, trail);
   //rearrange the trail and insert the destination 
   for (i = 5; i > 0; i--) {
      trail[i] = trail[i-1];
   }
   trail[0] = destination;
   if (drac_duplicated(trail) == 0) {
      isLegal = TRUE;
   } else {
      isLegal = FALSE;
   }
   #ifdef DEBUG
   printf("\nisLegal = %d\n", isLegal);
   #endif
   return isLegal;
}

static void map_howManyStep(DracView gameState, int steps[NUM_MAP_LOCATIONS]) {
   LocationID hunterLocation[NUM_HUNTER];
   int hunterSteps[NUM_HUNTER][NUM_MAP_LOCATIONS];
   int i, j;
   LocationID *path = malloc(sizeof(LocationID) * NUM_MAP_LOCATIONS);
   TransportID *tran = malloc(sizeof(TransportID) * NUM_MAP_LOCATIONS);
   for (i = 0; i < NUM_HUNTER; i++) {
      hunterLocation[i] = whereIs(gameState, i);
      for (j = 0; j < NUM_MAP_LOCATIONS; j++) {
         hunterSteps[i][j] = shortestPath(path, hunterLocation[i], j, TRUE, tran);
      }
   }

   for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
      for (j = 0; j < NUM_HUNTER; j++) {
         steps[i] += hunterSteps[j][i];
      }
   }
   free(path);
   free(tran);
}

