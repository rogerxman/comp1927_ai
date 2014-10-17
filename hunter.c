// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "HunterView.h"
#include <time.h>
static char *doubleBack(int i, LocationID trail[], HunterView gameState);
static char *randomCity(HunterView gameState);
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
                int ID = path[1];
                destination = idToabbrev(ID);
            } else {
                // It's a doubleback!
                switch (trail[known]){
                    case DOUBLE_BACK_1: known++; destination = doubleBack(known, trail, gameState);break;
                    case DOUBLE_BACK_2: known+=2; destination = doubleBack(known, trail, gameState);break;
                    case DOUBLE_BACK_3: known+=3; destination = doubleBack(known, trail, gameState);break;
                    case DOUBLE_BACK_4: known+=4; destination = doubleBack(known, trail, gameState);break;
                    case DOUBLE_BACK_5: known+=5; destination = doubleBack(known, trail, gameState);break;
                    default : destination = randomCity(gameState);
                }
            }
        } else {
            // Cannot find dracula, move to random city
            destination = randomCity(gameState);
        }

        // Register move
        registerBestPlay(destination,"I'm on holiday in Geneva");
    }         
}

static char *doubleBack(int i, LocationID trail[],HunterView gameState) {
   char * destination;
   if (i >= TRAIL_SIZE){
      destination = randomCity(gameState);
   } else {
      if (trail[i] >= MIN_MAP_LOCATION && trail[i] <= MAX_MAP_LOCATION){
         destination = idToabbrev(trail[i]);
      }  else{
         destination = randomCity(gameState);
      }
   }
   return destination;
}

static char *randomCity(HunterView gameState) {
    int numRandomRoad = 0;
    int numRandomRail = 0;
    int numRandomSea = 0;
    PlayerID player = whoAmI(gameState);
    LocationID *randomRoads = whereCanPlayerGo(gameState, &numRandomRoad, player, 1, 0, 0);
    LocationID *randomRails = whereCanPlayerGo(gameState, &numRandomRail, player, 0, 1, 0);
    LocationID *randomSeas = whereCanPlayerGo(gameState, &numRandomSea, player, 0, 0, 1);

    int numTotalRandom = numRandomRoad + numRandomRail + numRandomSea;
    LocationID randomCities[NUM_MAP_LOCATIONS] = {0};
    int i;
    for (i = 0; i < numTotalRandom; i++) {
        if (numRandomRoad > 0) {
            randomCities[i] = randomRoads[--numRandomRoad];
            //printf("[ROAD] Added %s to %d\n", idToName(randomRoads[numRandomRoad]), i);
        } else if (numRandomRail > 0) {
            randomCities[i] = randomRails[--numRandomRail];
            //printf("[RAIL] Added %s to %d\n", idToName(randomRails[numRandomRail]), i);
        } else if (numRandomSea > 0) {
            randomCities[i] = randomSeas[--numRandomSea];
            //printf("[SEAS] Added %s to %d\n", idToName(randomSeas[numRandomSea]), i);
        }
    }

    srand(time(NULL));
    int random = rand()%numTotalRandom;
    char *selectedCity = idToabbrev(randomCities[random]);
    return selectedCity;
}

/*static void  print (int *a){
	int i;
	for (i=0 ;i<NUM_MAP_LOCATIONS; i++) printf("%d ",a[i]);
		printf("\n");
}*///use for deugging