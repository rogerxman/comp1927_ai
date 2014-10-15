// HunterView.c ... HunterView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Map.h"

#define PASTPLAY_TURN   8
#define PASTPLAY_ROUND  40
#define PASTPLAY_STRLEN 8
#define TURN_PER_ROUND  5   
#include <stdio.h>

struct hunterView {
    Round      round;//FINISHED
    int        turn;
    int        score;//ROGER
    int        playerHealth[NUM_PLAYERS];//ROGER
    LocationID playerTrail[NUM_PLAYERS][TRAIL_SIZE];//ALAN
    int        endGame;//DANIEL
};

//static void update_endGame(HunterView);
static void get_turn (HunterView g, char *pastPlays);

HunterView newHunterView(char *pastPlays, PlayerMessage messages[])
{
    GameView gameview = newGameView(pastPlays,messages);
    HunterView hunterView = malloc(sizeof(struct hunterView));
    get_turn(hunterView,pastPlays);
    hunterView->round = getRound(gameview);
    hunterView->score = getScore(gameview);
    hunterView->endGame = FALSE;
    char *stringTemp = malloc(sizeof(char) * PASTPLAY_STRLEN);
    int i, j;
    for (i = 0; i < NUM_PLAYERS; i++){
       hunterView->playerHealth[i] = getHealth(gameview,i);
       for (j = 0; j < TRAIL_SIZE; j++){
         hunterView->playerTrail[i][j] = -1;
       }
    }
    
    for (i = 0; i < NUM_PLAYERS; i++){
       hunterView->playerHealth[i] = getHealth(gameview,i);
       int trail[TRAIL_SIZE];
       getHistory(gameview,i,trail);
       for (j = 0; j < TRAIL_SIZE; j++){
         hunterView->playerTrail[i][j] = trail[j];
       }
    }
    
    free(stringTemp);  
    disposeGameView(gameview); 
    return hunterView;
}
     
     
// Frees all memory previously allocated for the HunterView toBeDeleted
void disposeHunterView(HunterView toBeDeleted)
{
    //COMPLETE THIS IMPLEMENTATION
    free( toBeDeleted );
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round giveMeTheRound(HunterView currentView)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return currentView->round;
}

// Get the id of current player
PlayerID whoAmI(HunterView currentView)
{    
    return currentView->turn%NUM_PLAYERS-1;
}

// Get the current score
int giveMeTheScore(HunterView currentView)
{
    return currentView->score;
}

// Get the current health points for a given player
int howHealthyIs(HunterView currentView, PlayerID player)
{
    return currentView->playerHealth[player];
}

// Get the current location id of a given player
LocationID whereIs(HunterView currentView, PlayerID player)
{
    return currentView->playerTrail[player][0];
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(HunterView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    int i;
    for (i = 0; i < TRAIL_SIZE; i++){
       trail[i] = currentView->playerTrail[player][i];
    }
}

//// Functions that query the map to find information about connectivity

// What are my possible next moves (locations)
LocationID *whereCanIgo(HunterView currentView, int *numLocations, int road,int rail, int sea)
{
    LocationID exception[NUM_MAP_LOCATIONS] = {0}; 
    int me  = whoAmI(currentView);  
    int rail_hops = (currentView->round + me)%4; 
    LocationID * a = conditionalvertex(exception,numLocations,whereIs(currentView,me),rail_hops,road,rail,sea);
    return a;
}

// What are the specified player's next possible moves
LocationID *whereCanTheyGo(HunterView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
    LocationID exception[NUM_MAP_LOCATIONS] = {0}; 
    int rail_hops = (currentView->round + player)%4;   
    LocationID * a = conditionalvertex(exception,numLocations,whereIs(currentView,player),rail_hops,road,rail,sea);
    return a;
}

static void get_turn (HunterView g, char *pastPlays) {
   int stringLength;
   for (stringLength = 0; pastPlays[stringLength] != '\0'; stringLength++);
   g->turn = (stringLength + 1) / PASTPLAY_TURN + 1;
}

LocationID goStraightTo (LocationID from, LocationID to){
    LocationID toBeAnalyzed[NUM_MAP_LOCATIONS];
    TransportID trans[NUM_MAP_LOCATIONS];
    shortestPath(toBeAnalyzed,from,to,1,trans);
    return toBeAnalyzed[0];  
}

//returning number of locations of the shortest path considering the rails
int efficientPath (HunterView g,LocationID* path,LocationID to){
    PlayerID me = whoAmI(g);
    int round = giveMeTheRound(g);
    int rail_hops = (me + round)%4;
    LocationID from = whereIs(g,me);
    int numlocations = aobs(from,to,rail_hops,path);
    return numlocations;
}


