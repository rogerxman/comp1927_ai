// GameView.c ... GameView ADT implementation
// GROUP: LARD
// MEMBERS: Alan Lawrence Roger Daniel
// VERSION: BETA
// We are proud to release beta version for customers.
// Special Offer only $9.99 dollars (Don't pay $99.99 , 90% off)
// For early access and original soundtrack! 

#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include <stdio.h>

//#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#endif

#define PASTPLAY_TURN   8
#define PASTPLAY_ROUND  40
#define PASTPLAY_STRLEN 8
#define TURN_PER_ROUND  5    
#define NUM_HUNTERS     4
//nTraps:        Number of traps in specific location
//matureVampire: 0   for no vampire in the city
//               1-6 number of round for the vampire to mature
//playerHealth:  health for each player (refer to "Global.h")
//playerTrail :  CURRENT of the last six move of each player {CURRENT,LAST_ROUND_MOVE,x,x,x,x}
//               playerTrail[PLAYER_DRACULA][0] for dracula's current location

struct gameView {
    Round      round;//FINISHED
    int        turn;//FINISHED
    int        score;//ROGER
    int        nTraps[NUM_MAP_LOCATIONS];//LAWERENCE
    int        matureVampire[NUM_MAP_LOCATIONS];//LAWERENCE
    int        playerHealth[NUM_PLAYERS];//ROGER
    LocationID playerTrail[NUM_PLAYERS][TRAIL_SIZE];//ALAN
    int        endGame;//DANIEL
    int        isInHospital[NUM_HUNTERS];
};

static int isLandHere (LocationID place);
static int isSeaHere (LocationID place);
static PlayerID get_playerID (char *tobeRead);
static LocationID get_locationID (char *tobeRead);

static void get_string (char *pastPlays, char *buffer, int currentTurn);//FINISHED
static void get_round (GameView g, char *pastPlays);//FINISHED
static void get_turn (GameView g, char *pastPlays);//FINISHED

static void update_score(GameView g, char* stringTemp);//ROGER
static void update_nTraps(GameView g, char* stringTemp);//LAWERENCE
static void update_matureVampire(GameView g, char* stringTemp);//LAWERENCE
static void update_playerHealth(GameView g, char* stringTemp);//ROGER
static void update_playerTrail(GameView g, char* stringTemp);//ALAN
static void update_isInHospital(GameView g);
//static void update_endGame(GameView g);//DANIEL

// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
   #ifdef DEBUG
   printf("======START OF NEW GAME VIEW=======\n");
   #endif

   int i, j;
   //reading information for *pastPlays
   char* stringTemp = malloc(sizeof(char) * PASTPLAY_STRLEN);
   //PlayerMessage Has Been Omitted
   
   
   //Initialization
   GameView gameView = malloc(sizeof(struct gameView));
   gameView->round = 0;
   gameView->score = GAME_START_SCORE;
   
   //traps and vampire initialization
   for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
      gameView->nTraps[i] = 0;
      gameView->matureVampire[i] = 0;
   }
   //health initialization
   for (i = 0; i < NUM_PLAYERS; i++) {
      gameView->playerHealth[i] = GAME_START_HUNTER_LIFE_POINTS;
      for (j = 0; j < TRAIL_SIZE; j++) {
         gameView->playerTrail[i][j] = NOWHERE;
      }
   }
   gameView->playerHealth[PLAYER_DRACULA] = GAME_START_BLOOD_POINTS;
   gameView->endGame = FALSE;

   //Let's jam
   get_turn(gameView, pastPlays);
   get_round(gameView, pastPlays);
   int currentTurn;

   #ifdef DEBUG
   char* currentPlayer;
   Round currentRound;
   #endif

   for (currentTurn = 1; currentTurn < gameView->turn; currentTurn++) {
      get_string (pastPlays, stringTemp, currentTurn);
      #ifdef DEBUG
      switch (currentTurn % NUM_PLAYERS - 1) {
         case -1 : currentPlayer = "4 DRACULA"; break;
         case 0  : currentPlayer = "0 LORD GODALMING"; break;
         case 1  : currentPlayer = "1 DR SEWARD"; break;
         case 2  : currentPlayer = "2 VAN HELSING"; break;
         case 3  : currentPlayer = "3 MINA HARKER"; break;
      }
      currentRound = currentTurn / NUM_PLAYERS;
      printf("======================================\n");
      printf("Round %d\tPlayer %s\n", currentRound, currentPlayer);
      printf("ACTION    = %s\n\n", stringTemp);
      #endif

      assert(gameView->endGame == FALSE); //THIS LINE WOULD BE REPLACED LATER ON

      //THEN USE stringTemp
      //Use the stringTemp[0] to identify who you are
      //And change the corresponding information in the gameView
      //G: PLAYER_LORD_GODALMING
      //S: PLAYER_DR_SEWARD
      //H: PLAYER_VAN_HELSING
      //M: PLAYER_MINA_HARKER
      //D: PLAYER_DRACULA
      update_playerTrail(gameView, stringTemp); 
      update_playerHealth(gameView, stringTemp);
      update_isInHospital(gameView);
      update_score(gameView, stringTemp);
      update_nTraps(gameView, stringTemp);
      update_matureVampire(gameView, stringTemp);
//      update_endGame(gameView);
   }
   #ifdef DEBUG
   printf("======END OF NEW GAME VIEW=======\n\n\n");
   #endif
   free(stringTemp);
   return gameView;
}

static int isLandHere (LocationID place) {
   int result = FALSE;
   if (place >= MIN_MAP_LOCATION && place <= MAX_MAP_LOCATION) {
      result = (idToType(place) == LAND);
   } else if (place == CITY_UNKNOWN) {
      result = TRUE;
   }
   return result;
}

static int isSeaHere (LocationID place) {
   int result = FALSE;
   if (place >= MIN_MAP_LOCATION && place <= MAX_MAP_LOCATION) {
      result = (idToType(place) == SEA);
   } else if (place == SEA_UNKNOWN) {
      result = TRUE;
   }
   return result;
}

static void get_round (GameView g, char *pastPlays) {
   int stringLength;
   for (stringLength = 0; pastPlays[stringLength] != '\0'; stringLength++);
   g->round = (stringLength+1) / PASTPLAY_ROUND;
}

static void get_turn (GameView g, char *pastPlays) {
   int stringLength;
   for (stringLength = 0; pastPlays[stringLength] != '\0'; stringLength++);
   g->turn = (stringLength + 1) / PASTPLAY_TURN + 1;
}

//get specific string for each player
//char *buffer should always be "stringTemp" in the main gameView
//round starts from 0
//the last character of the string should always be '\0'
//For example; 
//"GBE.... SBR.... HLO.... MCA.... DSJ.V.. GSJVD.."
//get_string (pastPlays, stringTemp, 1, PLAYER_DRACULA)
//which will give you DSJ.V..'\0'
static void get_string (char *pastPlays, char *buffer, int currentTurn) {
   char *ptr;
   ptr = pastPlays + (currentTurn - 1) * PASTPLAY_STRLEN;
   int i;
   for (i = 0; i < PASTPLAY_STRLEN; i++) {
      buffer[i] = ptr[i];
   }
   buffer[i-1] = '\0';
}

static PlayerID get_playerID (char *tobeRead) {
   char playerName = tobeRead[0];
   PlayerID player;
   switch (playerName) {
      case 'G': player = PLAYER_LORD_GODALMING; break;
      case 'S': player = PLAYER_DR_SEWARD     ; break;
      case 'H': player = PLAYER_VAN_HELSING   ; break;
      case 'M': player = PLAYER_MINA_HARKER   ; break;
      case 'D': player = PLAYER_DRACULA       ; break;
   }
   return player;
}

static LocationID get_locationID (char *tobeRead) {
   char placeAbb[3] = {0,0,0};
   placeAbb[0] = tobeRead[1];
   placeAbb[1] = tobeRead[2];
   int placeID = abbrevToID(placeAbb);
   //For other locations
   if (placeID == NOWHERE) {
      switch(placeAbb[0]) {
         case 'C': placeID = CITY_UNKNOWN; break;
         case 'S': placeID = SEA_UNKNOWN ; break;
         case 'H': placeID = HIDE        ; break;
         case 'T': placeID = TELEPORT    ; break;
         case 'D': {
            switch(placeAbb[1]) {
               case '1': placeID = DOUBLE_BACK_1 ; break;
               case '2': placeID = DOUBLE_BACK_2 ; break;
               case '3': placeID = DOUBLE_BACK_3 ; break;
               case '4': placeID = DOUBLE_BACK_4 ; break;
               case '5': placeID = DOUBLE_BACK_5 ; break;
            }
         }
      }
   }
   return placeID;
}


//THE WORK WE NEED TO DO TOGETHER
static void update_playerTrail(GameView g, char* strlen){
   int i;
   char abbrev[3] = {strlen[1],strlen[2],'\0'};
   for (i = 5; i > 0; i--){
      switch (strlen[0]){
         case 'G':g->playerTrail[PLAYER_LORD_GODALMING][i] = g->playerTrail[PLAYER_LORD_GODALMING][i-1]; break;
         case 'S':g->playerTrail[PLAYER_DR_SEWARD][i] = g->playerTrail[PLAYER_DR_SEWARD][i-1]; break;
         case 'H':g->playerTrail[PLAYER_VAN_HELSING][i] = g->playerTrail[PLAYER_VAN_HELSING][i-1]; break;
         case 'M':g->playerTrail[PLAYER_MINA_HARKER][i] = g->playerTrail[PLAYER_MINA_HARKER][i-1]; break;
         case 'D':g->playerTrail[PLAYER_DRACULA][i] =  g->playerTrail[PLAYER_DRACULA][i-1]; break;
      }
   }
   

   switch (strlen[0]){
      case 'G':g->playerTrail[PLAYER_LORD_GODALMING][i] = abbrevToID(abbrev);break;
      case 'S':g->playerTrail[PLAYER_DR_SEWARD][i] = abbrevToID(abbrev);break;
      case 'H':g->playerTrail[PLAYER_VAN_HELSING][i] = abbrevToID(abbrev);break;
      case 'M':g->playerTrail[PLAYER_MINA_HARKER][i] = abbrevToID(abbrev);break;
      case 'D':{
         if (abbrev[0] == 'T' && abbrev[1] == 'P') {
            g->playerTrail[PLAYER_DRACULA][i] = TELEPORT;
         }  else if ( strlen[2] >='1' && strlen[2] <= '5' ){ 
            switch(strlen[2])
               case '1': g->playerTrail[PLAYER_DRACULA][i] = DOUBLE_BACK_1; break;
               case '2': g->playerTrail[PLAYER_DRACULA][i] = DOUBLE_BACK_2; break;
               case '3': g->playerTrail[PLAYER_DRACULA][i] = DOUBLE_BACK_3; break;
               case '4': g->playerTrail[PLAYER_DRACULA][i] = DOUBLE_BACK_4; break;
               case '5': g->playerTrail[PLAYER_DRACULA][i] = DOUBLE_BACK_5; break;
         }  else if(strlen[2] == '?'){
            if (strlen[1] == 'S') {
               g->playerTrail[PLAYER_DRACULA][i] = SEA_UNKNOWN;
            }  else {
               g->playerTrail[PLAYER_DRACULA][i] = CITY_UNKNOWN;   
            }                   
         }  else {
            g->playerTrail[PLAYER_DRACULA][i] = abbrevToID(abbrev);  
         }  
      } break;                      
   }
   #ifdef DEBUG
   printf("update_playerTrail\n");
   for (i = 0; i < NUM_PLAYERS; i++) {
      printf("Player %d : ", i);
      for (j = 0; j < TRAIL_SIZE; j++) {
         printf("%3d - ", g->playerTrail[i][j]);
      }
      printf("\n");
   }
   #endif
}

static void update_playerHealth(GameView g, char* stringTemp) {
   PlayerID player = get_playerID(stringTemp);
   LocationID location = get_locationID(stringTemp);
   LocationID lastLocation = g->playerTrail[player][1];
   int i;
   char ch;
   //For hunters
   if (player != PLAYER_DRACULA) {
      //DEAD?
      if (g->playerHealth[player] <= 0) {
         g->playerHealth[player] = GAME_START_HUNTER_LIFE_POINTS;
      }
      //REST?
      if (location == lastLocation && isLandHere(location)) {
         g->playerHealth[player] += LIFE_GAIN_REST;
         //Health check if exceed 9 points
         if (g->playerHealth[player] > GAME_START_HUNTER_LIFE_POINTS) {
            g->playerHealth[player] = GAME_START_HUNTER_LIFE_POINTS;
         }
      }
      //ENCOUNTERED?
      for (i = 3; i < PASTPLAY_STRLEN; i++) {
         ch = stringTemp[i];
         if (ch == 'T') {
            g->playerHealth[player] -= LIFE_LOSS_TRAP_ENCOUNTER;
         } else if (ch == 'D') {
            g->playerHealth[player] -= LIFE_LOSS_DRACULA_ENCOUNTER;
            g->playerHealth[PLAYER_DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER;
         } else {
            assert(ch == '.' || ch == 'V' || ch == '\0');
         }
      }

   } else /*turn % NUM_PLAYERS == PLAYER_DRACULA */ {
      //TORTURE IN THE SEA? RAPTURE IN CASTLE_DRACULA?
      if (isSeaHere(location)) {
         g->playerHealth[player] -= LIFE_LOSS_SEA;
      } else if (location == HIDE) {
         if (isSeaHere(g->playerTrail[player][1])) {
            g->playerHealth[player] -= LIFE_LOSS_SEA;
         } else if (lastLocation == CASTLE_DRACULA) {
            g->playerHealth[player] += LIFE_GAIN_CASTLE_DRACULA;
         }
      } else if (location >= DOUBLE_BACK_1 && location <= DOUBLE_BACK_5) {
         if (isSeaHere(g->playerTrail[player][location-HIDE])) {
            g->playerHealth[player] -= LIFE_LOSS_SEA;
         }
      } else if (location == CASTLE_DRACULA || location == TELEPORT) {
         g->playerHealth[player] += LIFE_GAIN_CASTLE_DRACULA;
      }
   }
   #ifdef DEBUG
   printf("update_playerHealth\n");
   printf("0 = %d\t1 = %d\t2 = %d\t3 = %d\t4 = %d\n", 
          g->playerHealth[0], g->playerHealth[1], g->playerHealth[2], g->playerHealth[3], g->playerHealth[4]);
   #endif
}

static void update_isInHospital(GameView g) {
   int i;
   for (i = 0; i < NUM_HUNTERS; i++) {
      if (g->playerHealth[i] <= 0) {
         g->isInHospital[i] = TRUE;
      } else {
         g->isInHospital[i] = FALSE;
      }
   }
}
static void update_score(GameView g, char* stringTemp) {
   PlayerID player = get_playerID(stringTemp);
   //1. Dracula moves, score -1.
   if (player == PLAYER_DRACULA) {
      g->score -= SCORE_LOSS_DRACULA_TURN;

      //2. If a vampire has matured
      if (stringTemp[6] == 'V') {
         g->score -= SCORE_LOSS_VAMPIRE_MATURES;
      }
   }

   //3. If hunters are dead, -> score -6 -> tp to hospital
   else /* player != PLAYER_DRACULA */ {
      if (g->playerHealth[player] <= 0) {
         g->score -= SCORE_LOSS_HUNTER_HOSPITAL;
      }
   }
}

static void update_nTraps(GameView g, char* stringTemp) {
   int i;
   PlayerID player = get_playerID(stringTemp);
   LocationID location = get_locationID(stringTemp);
   if (location >= MIN_MAP_LOCATION && location <= MAX_MAP_LOCATION){
      if (player == PLAYER_DRACULA) {
         // Trap placed
         if (stringTemp[3] == 'T') {
            g->nTraps[location]++;
         }
         // Trap has left the trail
         if (stringTemp[5] == 'M') {
            LocationID oldLocation = g->playerTrail[PLAYER_DRACULA][5];
            //oldLocation can equal to -1 because at round_0 Dracula can leave a trap.
            //Another consequence is when g->nTraps[-1]; it will modify g->score!
            //That's funny
            if (oldLocation >= -1 && oldLocation <= MAX_MAP_LOCATION){
               if (oldLocation == -1) {
                  oldLocation = 0;
               }
               g->nTraps[oldLocation]--;
            }
         }
      } else {
         int i;
         for (i = 3; i < PASTPLAY_STRLEN; i++) {
            // Trap encountered
            if (stringTemp[i] == 'T') {
               g->nTraps[location]--;
            }
         }
      }
      for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
         if (g->nTraps[i] < 0) {
            g->nTraps[i] = 0;
         }
      }
   }
}

static void update_matureVampire(GameView g, char* stringTemp) {
   int i;
   PlayerID player = get_playerID(stringTemp);
   LocationID location = get_locationID(stringTemp);
   if (location >= MIN_MAP_LOCATION && location <= MAX_MAP_LOCATION){
      if (player == PLAYER_DRACULA) {
         // Update rounds before vampires mature
         for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
            if (g->matureVampire[i] > 0) {
               g->matureVampire[i]--;
            }
         }
         // Immature vampire placed
         if (stringTemp[4] == 'V') {
            g->matureVampire[location] = 6;
         }
      } else {
         for (i = 3; i < PASTPLAY_STRLEN; i++) {
            // Immature vampire encountered
            if (stringTemp[i] == 'V') {
               g->matureVampire[location] = 0;
            }
         }
      }
      for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
         if (g->matureVampire[i] < 0) {
            g->matureVampire[i] = 0;
         }
      }
   }
}

// Frees all memory previously allocated for the GameView toBeDeleted
void disposeGameView(GameView toBeDeleted)
{
    free( toBeDeleted );
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round getRound(GameView currentView)
{
    return currentView->round;
}

// Get the id of current player - ie whose turn is it?
PlayerID getCurrentPlayer(GameView currentView)
{
    return ((currentView->turn-1) % NUM_PLAYERS);
}

// Get the current score
int getScore(GameView currentView)
{
    return currentView->score;
}

// Get the current health points for a given player
int getHealth(GameView currentView, PlayerID player)
{
    return (currentView->playerHealth[player]);
}

// Get the current location id of a given player
LocationID getLocation(GameView currentView, PlayerID player)
{
    return (currentView->playerTrail[player][0]);
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void getHistory(GameView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
   int i;
   for (i = 0; i < TRAIL_SIZE; i++) {
      trail[i] = currentView->playerTrail[player][i];
   }
}

//// Functions that query the map to find information about connectivity

// Returns an array of LocationIDs for all directly connected locations
// Remember to free after using

LocationID *connectedLocations(GameView currentView, int *numLocations,
                               LocationID from, PlayerID player, Round round,
                               int road, int rail, int sea)
{  
   int i;
   LocationID exception[NUM_MAP_LOCATIONS] = {0};
   if (player == PLAYER_DRACULA){
      assert(rail == 0);
      for (i = 0; i < 6; i++)
         exception[abbrevToID("JM")] = 1;
   }
   int rail_hops = (round + player)%4;
   LocationID *a = conditionalvertex(exception,numLocations,from,rail_hops,road,rail,sea);
   return a;
}

int trapsCount (GameView g, LocationID location) {
   return g->nTraps[location];
}

int vampireAge (GameView g, LocationID location) {
   return g->matureVampire[location];
}


