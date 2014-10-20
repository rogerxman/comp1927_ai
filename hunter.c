// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "HunterView.h"
#include <time.h>

#define NEW_CITY_MAX_TRIES 25

static char *doubleBack(int i, LocationID trail[], HunterView gameState);
static char *newCity(HunterView gameState, PlayerID player, int turnLimit);
static int getAdjacentCities(HunterView gameState, PlayerID player, LocationID adjacentCities[]);
static char *randomCity(HunterView gameState, PlayerID player);
//static void  print(int *a);

void decideHunterMove(HunterView gameState) {
    LocationID trail[TRAIL_SIZE];
    giveMeTheTrail(gameState, PLAYER_DRACULA, trail);
    PlayerID hunterId = whoAmI(gameState);
    LocationID currentLocation = whereIs(gameState, hunterId);
    // Game just started... get a starting position
    if (currentLocation == UNKNOWN_LOCATION) {
        switch (hunterId) {
            case 0: registerBestPlay("LO","I'm on holiday in Geneva"); break;
            case 1: registerBestPlay("TO","I'm on holiday in Geneva"); break;
            case 2: registerBestPlay("MU","I'm on holiday in Geneva"); break;
            case 3: registerBestPlay("SO","I'm on holiday in Geneva"); break;
        }
    } else {
        // Check if we have found dracula's trail
        char *destination;
        int known = -1;
        int i = 0;
        // Look for one of dracula's trails
        for (i = 0; i < TRAIL_SIZE; i++) {
            if (trail[i] != CITY_UNKNOWN && trail[i] != SEA_UNKNOWN && trail[i] != UNKNOWN_LOCATION) {
                known = i;
                break;
            }
        }
        if (known != -1) {
            // Found one of dracula's trails
            if (trail[known] <= MAX_MAP_LOCATION && trail[known] >= MIN_MAP_LOCATION){
                // It's a location... follow him!
                LocationID path[NUM_MAP_LOCATIONS] = {0};
                efficientPath (gameState,path,trail[known]);
                // we have reached the city where dracula was, so to others
                 if (currentLocation == path[0]){
                    destination = newCity(gameState, whoAmI(gameState), TRAIL_SIZE);;
                 }  else{
                    destination = idToabbrev(path[1]); 
                 }               
            } else {
                // It's a doubleback!
                switch (trail[known]){
                    case DOUBLE_BACK_1: known++; destination = doubleBack(known, trail, gameState);break;
                    case DOUBLE_BACK_2: known+=2; destination = doubleBack(known, trail, gameState);break;
                    case DOUBLE_BACK_3: known+=3; destination = doubleBack(known, trail, gameState);break;
                    case DOUBLE_BACK_4: known+=4; destination = doubleBack(known, trail, gameState);break;
                    case DOUBLE_BACK_5: known+=5; destination = doubleBack(known, trail, gameState);break;
                    default : destination = newCity(gameState, hunterId, TRAIL_SIZE);
                }
            }
        } else {
            // Cannot find dracula, move to random city
            destination = newCity(gameState, hunterId, TRAIL_SIZE);
        }

        // Register move
        registerBestPlay(destination,"I found you HaHaHaHa~");
    }
}

static char *doubleBack(int i, LocationID trail[], HunterView gameState) {
   char * destination;
   PlayerID hunterId = whoAmI(gameState);
    LocationID currentLocation = whereIs(gameState, hunterId);
   LocationID path[NUM_MAP_LOCATIONS] = {0};
   
   if (i >= TRAIL_SIZE){
      destination = newCity(gameState, whoAmI(gameState), TRAIL_SIZE);
   } else {
// found dracula again
      if (trail[i] >= MIN_MAP_LOCATION && trail[i] <= MAX_MAP_LOCATION){
         efficientPath(gameState,path,trail[i]);
         if (currentLocation == path[0]){
            destination = newCity(gameState, whoAmI(gameState), TRAIL_SIZE);;
         }  else{
            destination = idToabbrev(path[1]); 
         }      
      }  else{
         destination = newCity(gameState, whoAmI(gameState), TRAIL_SIZE);
      }
   }
   return destination;
}

static char *newCity(HunterView gameState, PlayerID player, int turnLimit) {
    // turnLimit:
    // If you just want any city that was not your previous city, turnLimit = 1 (turn ago)
    // So if you want a new city that you have not been to in the past 3 turns.. then turnLimit = 3 (turns ago)
    LocationID trail[TRAIL_SIZE];
    giveMeTheTrail(gameState, player, trail);

    if (turnLimit > TRAIL_SIZE) {
        turnLimit = TRAIL_SIZE;
    }

    LocationID adjacentCities[NUM_MAP_LOCATIONS] = {0};
    int numLocations = getAdjacentCities(gameState, player, adjacentCities);

    char *city = NULL;
    int tries = 0;
    srand(time(NULL));
    // Find new city while we haven't passed max tries
    while (city == NULL && tries < NEW_CITY_MAX_TRIES) {
        // Get random index
        int random = rand()%numLocations;
        int visited = FALSE;
        int i;
        // Check if we 'visited' that city already
        for (i = 0; i < turnLimit; i++) {
            if (adjacentCities[random] == trail[i]) {
                visited = TRUE;
            }
        }
        if (!visited) {
            // Found a new city
            city = idToabbrev(adjacentCities[random]);
        } // else try again
        tries++;
    }

    if (city == NULL) {
        // Cannot find any new cities
        if (turnLimit > 1) {
            // Continue to try find a new city, but with 1 less turnLimit
            city = newCity(gameState, player, turnLimit - 1);
        } else {
            // Cannot find anything, just go to random city
            city = randomCity(gameState, player);
        }
    }

    return city;
}

static char *randomCity(HunterView gameState, PlayerID player) {
    LocationID adjacentCities[NUM_MAP_LOCATIONS] = {0};
    int numLocations = getAdjacentCities(gameState, player, adjacentCities);

    srand(time(NULL));
    int random = rand()%numLocations;
    char *selectedCity = idToabbrev(adjacentCities[random]);
    return selectedCity;
}

static int getAdjacentCities(HunterView gameState, PlayerID player, LocationID adjacentCities[]) {
    int numAdjacentRoad = 0;
    int numAdjacentRail = 0;
    int numAdjacentSea = 0;
    LocationID *adjacentRoads = whereCanPlayerGo(gameState, &numAdjacentRoad, player, 1, 0, 0);
    LocationID *adjacentRails = whereCanPlayerGo(gameState, &numAdjacentRail, player, 0, 1, 0);
    LocationID *adjacentSeas = whereCanPlayerGo(gameState, &numAdjacentSea, player, 0, 0, 1);

    int numTotalAdjacent = numAdjacentRoad + numAdjacentRail + numAdjacentSea;
    int i;
    for (i = 0; i < numTotalAdjacent; i++) {
        if (numAdjacentRoad > 0) {
            adjacentCities[i] = adjacentRoads[--numAdjacentRoad];
            //printf("[ROAD] Added %s to %d\n", idToName(adjacentRoads[numAdjacentRoad]), i);
        } else if (numAdjacentRail > 0) {
            adjacentCities[i] = adjacentRails[--numAdjacentRail];
            //printf("[RAIL] Added %s to %d\n", idToName(adjacentRails[numAdjacentRail]), i);
        } else if (numAdjacentSea > 0) {
            adjacentCities[i] = adjacentSeas[--numAdjacentSea];
            //printf("[SEAS] Added %s to %d\n", idToName(adjacentSeas[numAdjacentSea]), i);
        }
    }

    return numTotalAdjacent;
}

/*static void  print (int *a){
	int i;
	for (i=0 ;i<NUM_MAP_LOCATIONS; i++) printf("%d ",a[i]);
		printf("\n");
}*///use for deugging
