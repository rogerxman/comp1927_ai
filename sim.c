//simulator.c
//Group: LARD
//By : Roger Tsang

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Game.h"
#include "Globals.h"
#include "GameView.h"
#include "dracula.h"
#include "hunter.h"

#define MOVE_SIZE 3

// The minimum static globals I can get away with
static char latestPlay[MOVE_SIZE] = "";
static char latestMessage[MESSAGE_SIZE] = "";

void stringGenerator (char* stringTemp, PlayerID player, LocationID location);

int main(int argc, char *argv[])
{
   char* plays = malloc(sizeof(char) * GAME_START_SCORE*NUM_PLAYERS*8);
   PlayerMessage msgs[GAME_START_SCORE*NUM_PLAYERS] = {""};
   char* buffer = malloc(sizeof(char) * 9);

   //Arche State
   plays[0] = '\0';
   GameView godState;
   DracView dracState;
   HunterView huntState;
   Round game_round; 
   int   game_score = GAME_START_SCORE; 
   int   game_player;
   int   drac_health = 40;
   while (game_score > 0 && drac_health > 0 && game_round < 10) {
      godState = newGameView(plays, msgs);
      game_round = getRound(godState);
      game_score = getScore(godState);
      game_player = getCurrentPlayer(godState);
      drac_health = getHealth(godState, 4);
      if (game_player == 4) {
         dracState = newDracView(plays,msgs);
         stringGenerator(buffer, game_player, decideDraculaMove(dracState));
         strcat(plays, buffer);
         disposeDracView(dracState);
      } else {
         hunterState = newHunterView(plays, msgs);
         stringGenerator(buffer, game_player, decideHunterMove(hunterState));
         strcat(plays, buffer);
         disposeHunterView(hunterState);
      }
      plays[game_round*40 + game_player*8 + 7] = '\0';
      plays[game_round*40 + game_player*8 + 8] = 'X';
      printf("%s\n", plays);
   }
   if (drac_health > 0) {
      printf("dracula win !!\n");
   } else {
      printf("hunters win !!\n");
   }
   return EXIT_SUCCESS;
}

// Saves characters from play (and appends a terminator)
// and saves characters from message (and appends a terminator)
void registerBestPlay (char *play, PlayerMessage message) {
   strncpy(latestPlay, play, MOVE_SIZE-1);
   latestPlay[MOVE_SIZE-1] = '\0';
     
   strncpy(latestMessage, message, MESSAGE_SIZE);
   latestMessage[MESSAGE_SIZE-1] = '\0';
}

void stringGenerator (char* stringTemp, PlayerID player, char* location) {
   switch (player) {
      case '0': stringTemp[0] = 'G'; break;
      case '1': stringTemp[0] = 'S'; break;
      case '2': stringTemp[0] = 'H'; break;
      case '3': stringTemp[0] = 'M'; break;
      case '4': stringTemp[0] = 'D'; break;
   }
   stringTemp[1] = location[0];
   stringTemp[2] = location[1];
   stringTemp[3] = '.';
   stringTemp[4] = '.';
   stringTemp[5] = '.';
   stringTemp[6] = '.';
   stringTemp[7] = ' ';
   stringTemp[8] = '\0';
}