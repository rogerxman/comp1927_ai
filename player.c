/**
 * Runs a player's game turn ...
 *
 * Can produce either a Hunter player or a Dracula player
 * depending on the setting of the I_AM_DRACULA #define
 *
 * This is a dummy version of the real player.c used when
 * you submit your AIs. It is provided so that you can
 * test whether your code is likely to compile ...
 *
 * Note that it used to drive both Hunter and Dracula AIs.
 * It first creates an appropriate view, and then invokes the
 * relevant decide-my-move function, which should use the 
 * registerBestPlay() function to send the move back.
 *
 * The real player.c applies a timeout, so that it will halt
 * decide-my-move after a fixed amount of time, if it doesn't
 * finish first. The last move registered will be the one
 * used by the game engine. This version of player.c won't stop
 * your decide-my-move function if it goes into an infinite
 * loop. Sort that out before you submit.
 *
 * Based on the program by David Collien, written in 2012
 */

//latest update by roger tsang on 10/14/2014

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Game.h"
#ifdef I_AM_DRACULA
#include "DracView.h"
#include "dracula.h"
#else
#include "HunterView.h"
#include "hunter.h"
#endif

// moves given by registerBestPlay are this long (including terminator)
#define MOVE_SIZE 3

// The minimum static globals I can get away with
static char latestPlay[MOVE_SIZE] = "";
static char latestMessage[MESSAGE_SIZE] = "";

int main(int argc, char *argv[])
{
#ifdef I_AM_DRACULA
   DracView gameState;
   int i;
   char input[3];
   char *plays = malloc(sizeof(char) * 80 * 366);
   plays[0] = '\0';
   char *player0 = "G";
   char *player1 = ".... S";
   char *player2 = ".... H";
   char *player3 = ".... M";
   char *player4 = "....";
   char *player5 = " D";
   char *player6 = "T...\n";
   strcpy(plays, player0);
   while(1) {
      for (i = 1; i < 5; i++) {
         scanf("%s", input);
         strcat(plays, input);
         switch(i) {
            case 1: strcat(plays, player1); break;
            case 2: strcat(plays, player2); break;
            case 3: strcat(plays, player3); break;
            case 4: strcat(plays, player4); break;
         }
      }
         printf("plays = %s\n", plays);
         PlayerMessage msgs[366] = {""};
         gameState = newDracView(plays,msgs);
         decideDraculaMove(gameState);
         disposeDracView(gameState);
         printf("Dracula Moves To: %s\n", latestPlay);
         strcat(plays, player5);
         strcat(plays, latestPlay);
         strcat(plays, player6);
         strcat(plays, player0);
   }
#else
   HunterView gameState;
   char *plays = "GLO.... STO.... HMU.... MSO.... DC?.V.. GPL.... SSR.... HNU.... MBC.... DMAT... GEC.... SMATD.. HST.... MBE.... DC?T... GAO....";
   PlayerMessage msgs[3] = { "", "", "" };
   gameState = newHunterView(plays,msgs);
   decideHunterMove(gameState);
   disposeHunterView(gameState);
#endif 
   printf("Move: %s, Message: %s\n", latestPlay, latestMessage);
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
