// DracView.c ... DracView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "DracView.h"
#include "Map.h"

#ifdef DEBUG
#include <stdio.h>
#endif

#define PASTPLAY_TURN   8
#define PASTPLAY_ROUND  40
#define PASTPLAY_STRLEN 8
#define TURN_PER_ROUND  5    

struct dracView {
    Round      round;//FINISHED
    int        score;//ROGER
    int        nTraps[NUM_MAP_LOCATIONS];//LAWERENCE
    int        matureVampire[NUM_MAP_LOCATIONS];//LAWERENCE
    int        playerHealth[NUM_PLAYERS];//ROGER
    LocationID playerTrail[NUM_PLAYERS][TRAIL_SIZE];//ALAN
    int        endGame;//DANIEL
};

//static void update_endGame(DracView g);//DANIEL
// Creates a new DracView to summarise the current state of the game
DracView newDracView(char *pastPlays, PlayerMessage messages[]) 
{
   #ifdef DEBUG
   printf("======START OF NEW GAME VIEW=======\n");
   #endif

   GameView gameview = newGameView(pastPlays,messages);
   DracView dracView = malloc(sizeof(struct dracView));
   dracView->round = getRound(gameview);
   dracView->score = getScore(gameview);
   dracView->endGame = FALSE;
   char *stringTemp = malloc(sizeof(char) * PASTPLAY_STRLEN);
   int i, j;
   for (i = 0; i < NUM_PLAYERS; i++){
      dracView->playerHealth[i] = getHealth(gameview,i);
      for (j = 0; j < TRAIL_SIZE; j++){
        dracView->playerTrail[i][j] = -1;
      }
   }
   
   for (i = 0; i < NUM_PLAYERS; i++){
      dracView->playerHealth[i] = getHealth(gameview,i);
      int trail[TRAIL_SIZE];
      getHistory(gameview,i,trail);
      for (j = 0; j < TRAIL_SIZE; j++){
        dracView->playerTrail[i][j] = trail[j];
      }
   }

   for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
      dracView->nTraps[i] = trapsCount(gameview, i);
      dracView->matureVampire[i] = vampireAge(gameview, i);
   }   

   #ifdef DEBUG
   printf("======END OF NEW GAME VIEW=======\n\n\n");
   #endif
   free(stringTemp);
   disposeGameView(gameview);
   return dracView;
}


// Frees all memory previously allocated for the DracView toBeDeleted
void disposeDracView(DracView toBeDeleted)
{
    //COMPLETE THIS IMPLEMENTATION
    free( toBeDeleted );
}

//// Functions to return simple information about the current state of the game

// Get the current round
Round giveMeTheRound(DracView currentView)
{
    return currentView->round;
}

// Get the current score
int giveMeTheScore(DracView currentView)
{
    return currentView->score;
}

// Get the current health points for a given player
int howHealthyIs(DracView currentView, PlayerID player)
{
    return currentView->playerHealth[player];
}

// Get the current location id of a given player
LocationID whereIs(DracView currentView, PlayerID player)
{
    return currentView->playerTrail[player][0];
}

// Get the most recent move of a given player
void lastMove(DracView currentView, PlayerID player,
                 LocationID *start, LocationID *end)
{
   *start = currentView->playerTrail[player][1];
   *end   = currentView->playerTrail[player][0];
   return;
}

// Find out what minions are placed at the specified location
void whatsThere(DracView currentView, LocationID where,
                         int *numTraps, int *numVamps)
{
   *numTraps = currentView->nTraps[where];
   *numVamps = (currentView->matureVampire[where] > 0);
   return;
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(DracView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
   int i;
   for(i = 0; i < TRAIL_SIZE; i++) {
      trail[i] = currentView->playerTrail[player][i];
   }
}

//// Functions that query the map to find information about connectivity

// What are my (Dracula's) possible next moves (locations)
LocationID *whereCanIgo(DracView currentView, int *numLocations, int road, int sea)
{
   int i;
   Round currentRound = currentView->round;
   LocationID currentPlace = currentView->playerTrail[PLAYER_DRACULA][0];
   LocationID exception[NUM_MAP_LOCATIONS ] = {0};
   for (i = 0; i < 6; i++)
      exception[currentView->playerTrail[PLAYER_DRACULA][i]] = 1;
   exception[abbrevToID("JM")] = 1;

   LocationID * result = conditionalvertex(exception, numLocations, currentPlace, currentRound, road, FALSE, sea);
   return result;
}

// What are the specified player's next possible moves
LocationID *whereCanTheyGo(DracView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
   Round currentRound = (currentView->round + player) % 4;
   LocationID currentPlace = currentView->playerTrail[player][0];
   LocationID exception[NUM_MAP_LOCATIONS ] = {0};
   LocationID * result = conditionalvertex(exception, numLocations, currentPlace, currentRound, road, rail, sea);
   return result;
}