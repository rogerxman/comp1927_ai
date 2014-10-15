// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "HunterView.h"
#include <time.h>
static char* doubleBack(int i, LocationID trail[], HunterView gameState);
static char* randomCity(HunterView gameState);
//static void  print(int *a);

void decideHunterMove(HunterView gameState)
{
    LocationID trail[TRAIL_SIZE];
    giveMeTheTrail(gameState,PLAYER_DRACULA,trail);
    int i = 0;
    PlayerID me = whoAmI(gameState);
    LocationID now = whereIs(gameState,me);
    if (now == UNKNOWN_LOCATION){
       switch (me){
          case 0: registerBestPlay("LO","I'm on holiday in Geneva");break;
          case 1: registerBestPlay("TO","I'm on holiday in Geneva");break;
          case 2: registerBestPlay("MU","I'm on holiday in Geneva");break;
          case 3: registerBestPlay("SO","I'm on holiday in Geneva");break;
       }
    }  else{
       char * destination;
       while (trail[i] == CITY_UNKNOWN || trail[i] == SEA_UNKNOWN || trail[i] == UNKNOWN_LOCATION) i++;
       if ( i < TRAIL_SIZE ){
          if (trail[i] <= MAX_MAP_LOCATION && trail[i] >= MIN_MAP_LOCATION){
             LocationID path[NUM_MAP_LOCATIONS];
             efficientPath (gameState,path,trail[i]);
             int ID = path[1];
             destination = idToabbrev(ID);
             registerBestPlay(destination,"I'm on holiday in Geneva");
          }  else{
             switch (trail[i]){
                case DOUBLE_BACK_1: i++; destination = doubleBack(i,trail,gameState);break;
                case DOUBLE_BACK_2: i+=2; destination = doubleBack(i,trail,gameState);break;
                case DOUBLE_BACK_3: i+=3; destination = doubleBack(i,trail,gameState);break;
                case DOUBLE_BACK_4: i+=4; destination = doubleBack(i,trail,gameState);break;
                case DOUBLE_BACK_5: i+=5; destination = doubleBack(i,trail,gameState);break;
                default : destination = randomCity(gameState);
             }
          }
       }  else {
          destination = randomCity(gameState);
          registerBestPlay(destination,"I'm on holiday in Geneva");
       }
   }         
}

static char* doubleBack(int i, LocationID trail[],HunterView gameState){
   char * destination;
   if (i >= TRAIL_SIZE){
      destination = randomCity(gameState);
   }  else {
      if (trail[i] >= MIN_MAP_LOCATION && trail[i] <= MAX_MAP_LOCATION){
         destination = idToabbrev(trail[i]);
      }  else{
         destination = randomCity(gameState);
      }
   }
   return destination;
}

static char* randomCity(HunterView gameState){
   int  num;
   int numa = 0;
   int numb = 0;
   int numc = 0;
   int visited[NUM_MAP_LOCATIONS] = {0};
   LocationID* a = whereCanIgo(gameState,&numa,1,0,0);
   LocationID* b = whereCanIgo(gameState,&numb,0,1,0);
   LocationID* c = whereCanIgo(gameState,&numc,0,0,1);
   num = numa + numb + numc;
   LocationID combine[NUM_MAP_LOCATIONS] = {0};
   int i;
   numa--;
   numb--;
   numc--;
   for  (i = 0; numa >= 0; i++){
   	    if (!visited[a[numa]]){
       	   combine[i] = a[numa--];
       	   visited[combine[i]] = 1;
        }else {
       	   numa--;
       	   i--;
        }
   }
   for (; numb >= 0; i++){
        if (!visited[b[numb]]){
       	   combine[i] = b[numb--];
       	   visited[combine[i]] = 1;
        }else {
       	   numb--;
       	   i--;
        }
   }
   for (; numc >= 0; i++){
        if (!visited[c[numc]]){
       	   combine[i] = c[numc--];
       	   visited[combine[i]] = 1;
        }else {
       	   numc--;
       	   i--;
        }
   }
  //print(combine);
   srand(time(NULL));
   int r = rand();
   int R = r%num;
   char *tobereturn = idToabbrev(combine[R]);
   return tobereturn;
}

/*static void  print (int *a){
	int i;
	for (i=0 ;i<NUM_MAP_LOCATIONS; i++) printf("%d ",a[i]);
		printf("\n");
}*///use for deugging