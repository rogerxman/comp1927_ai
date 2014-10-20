// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI
// By: Roger Tsang
// Version: Beta

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "Map.h"
#include "DracView.h"
#include "Queue.h"

//#define DEBUG

#define SIZE_TAUNT MESSAGE_SIZE
#define DOUBLE_BACK_0 102
#define NUM_HUNTER  4
#define UNASSIGNED -1
#define ERROR      -2

//UPDATE: ALPHA
static LocationID drac_nextLocation(DracView gameState, LocationID destination);
static LocationID drac_idealPlace(DracView gameState);
//static LocationID drac_randomPlace(DracView gameState);
static LocationID drac_autoNextMove(DracView gameState);
//static int drac_duplicated(LocationID trail[TRAIL_SIZE]);
//static int drac_isLegalMove(DracView gameState, LocationID destination);
static void map_howManyStep(DracView gameState, int steps[NUM_MAP_LOCATIONS]);

//UPDATE: BETA
//STRATEGE: ENABLE HI D1 D2 D3 D4 D5
//COMMENT: I COMBINED drac_duplicated and drac_isLegalMove to drac_autoHIDX
static LocationID drac_hasHI(LocationID trail[TRAIL_SIZE]);
static LocationID drac_hasDX(LocationID trail[TRAIL_SIZE]);
static LocationID drac_autoHIDX(LocationID trail[TRAIL_SIZE], LocationID input);
static LocationID drac_drunk(LocationID trail[TRAIL_SIZE], LocationID currentLocation);

void decideDraculaMove(DracView gameState)
{
   #ifdef DEBUG
   printf("decideDraculaMove\n");
   #endif
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

   //3rd Step: Transformer
   if (placeToGo >= MIN_MAP_LOCATION && placeToGo <= MAX_MAP_LOCATION) {
      registerBestPlay(idToabbrev(placeToGo), "Dracula");      
   } else {
      switch(placeToGo) {
         case HIDE: registerBestPlay("HI", "Dracula"); break;
         case DOUBLE_BACK_1: registerBestPlay("D1", "Dracula"); break;
         case DOUBLE_BACK_2: registerBestPlay("D2", "Dracula"); break;
         case DOUBLE_BACK_3: registerBestPlay("D3", "Dracula"); break;
         case DOUBLE_BACK_4: registerBestPlay("D4", "Dracula"); break;
         case DOUBLE_BACK_5: registerBestPlay("D5", "Dracula"); break;
      }
   }
}

static LocationID drac_nextLocation(DracView gameState, LocationID destination) {
   #ifdef DEBUG
   printf("drac_nextLocation\n");
   #endif
   int myLocation = whereIs(gameState, PLAYER_DRACULA);
   int nextLocation;
   int numLocations;
   LocationID *path = malloc(sizeof(LocationID) * NUM_MAP_LOCATIONS);
   LocationID exception[NUM_MAP_LOCATIONS] = {0};
   exception[ST_JOSEPH_AND_ST_MARYS] = 1;
   path = conditionalvertex(exception, &numLocations, myLocation, 0, TRUE, FALSE, FALSE);
   nextLocation = path[0];
   free(path);
   return nextLocation;
}

static LocationID drac_idealPlace(DracView gameState) {
   #ifdef DEBUG
   printf("drac_idealPlace\n");
   #endif
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

//Beta update: Rewrite with drac_autoHIDX
static LocationID drac_autoNextMove(DracView gameState) {
   #ifdef DEBUG
   printf("drac_autoNextMove\n");
   #endif
   LocationID *myTrail = malloc(sizeof(LocationID) * TRAIL_SIZE);
   giveMeTheTrail(gameState, PLAYER_DRACULA, myTrail);
   LocationID myLocation = myTrail[0];
   LocationID idealDes = drac_nextLocation(gameState, drac_idealPlace(gameState));
   LocationID finalDes;
   finalDes = drac_autoHIDX(myTrail, idealDes);
   if (finalDes == ERROR) {
      finalDes = drac_drunk(myTrail, myLocation);
   }
   return finalDes;
}
/*
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
   #ifdef DEBUG
   printf("duplicate = %d\n", duplicate);
   #endif
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
   #ifdef DEBUG
   printf("\nisLegal = %d\n", isLegal);
   #endif
   #endif
   return isLegal;
}
*/
static void map_howManyStep(DracView gameState, int steps[NUM_MAP_LOCATIONS]) {
   #ifdef DEBUG
   printf("map_howManyStep\n");
   #endif
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
//This function is not yet completed
static LocationID drac_hasHI(LocationID trail[TRAIL_SIZE]) {
   #ifdef DEBUG
   printf("drac_hasHI\n");
   #endif
   /*
   int i;
   int check = FALSE;
   for (i = 0; i < TRAIL_SIZE && check == FALSE; i++) {
      if (trail[i] == HIDE) {
         check = TRUE;
      }
   }
   return check;
   */
   return TRUE;
}

static LocationID drac_hasDX(LocationID trail[TRAIL_SIZE]) {
   #ifdef DEBUG
   printf("drac_hasDX\n");
   #endif
   int i, j;
   int check = FALSE;
   for (i = 0; i < TRAIL_SIZE; i++) {
   }
   for (i = 0; i < TRAIL_SIZE - 1 && check == FALSE; i++) {
      for (j = i + 1; j < TRAIL_SIZE && check == FALSE; j++) {
         if (trail[i] == trail[j] && trail[i] != -1 && trail[j] != -1) {
            check = TRUE;
         }
      }
   }
   return check;
}

static LocationID drac_autoHIDX(LocationID trail[TRAIL_SIZE], LocationID input) {
   #ifdef DEBUG
   printf("drac_autoHIDX\n");
   #endif
   int i, wantHide = FALSE, wantDoubleback = FALSE;
   LocationID returnLocation = UNASSIGNED;
   LocationID testTrail[TRAIL_SIZE];
   //Routine: 1-Step: Can I use HIDE?
   //         2-Step: Otherwise I will use DOUBLE_BACK_X
   //         3-Step: If not, I will return ERROR (Job should be done by islegal)
   
   //Shifting the trail -1->
   testTrail[0] = input;
   for (i = 1; i < 6; i++) {
      testTrail[i] = trail[i-1];
      if (testTrail[i] == input) {
         wantDoubleback = DOUBLE_BACK_0 + i;
      }
   }
   if (testTrail[1] == input) {
      wantHide = TRUE;
   }

   //Directly pass the location
   if (wantHide == FALSE && wantDoubleback == FALSE) {
      returnLocation = input;
   }
   //Check: Can i use HIDE?
   else if (wantHide != FALSE && !drac_hasHI(trail)) {
      returnLocation = HIDE;
   } 
   //Check: Can i use DOUBLE_BACK_X
   else if (wantDoubleback != FALSE && !drac_hasDX(trail)) {
      returnLocation = wantDoubleback;
   }
   //Warning: This is an illegal move
   else {
      returnLocation = ERROR;
   }
   return returnLocation;
}

static LocationID drac_drunk(LocationID trail[TRAIL_SIZE], LocationID currentLocation) {
   #ifdef DEBUG
   printf("drac_drunk\n");
   #endif
   int i, numLocations;
   LocationID decidedLocation = ERROR;
   LocationID *placeICanReach = malloc(sizeof(placeICanReach) * NUM_MAP_LOCATIONS);
   //City Search First
   Drac_whereCanIGo(placeICanReach, &numLocations, currentLocation, TRUE, FALSE);
   for (i = 0; i < numLocations && decidedLocation == ERROR; i++) {
      decidedLocation = drac_autoHIDX(trail, placeICanReach[i]);
   }
   //Sea Search 
   if (decidedLocation == ERROR) {
      Drac_whereCanIGo(placeICanReach, &numLocations, currentLocation, FALSE, TRUE);
      for (i = 0; i < numLocations && decidedLocation == ERROR; i++) {
         decidedLocation = drac_autoHIDX(trail, placeICanReach[i]);
      }
   }
   free(placeICanReach);
   return decidedLocation;
}