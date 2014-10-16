// Map.h ... interface to Map data type

#ifndef MAP_H
#define MAP_H

#include "Places.h"

typedef struct edge{
    LocationID  start;
    LocationID  end;
    TransportID type;
} Edge;

// graph representation is hidden 
typedef struct MapRep *Map; 

// operations on graphs 
Map  newMap();  
void disposeMap(Map g); 
void showMap(Map g); 
int  numV(Map g);
int  numE(Map g, TransportID t);
LocationID *conditionalvertex(int* exception, int *numLocations, LocationID from, int railHops,
                               int road, int rail, int sea);
//shortest path regardless of taking trains to go pass more than 1 stations
int shortestPath (LocationID* path,LocationID start,LocationID end,int isHunter,TransportID trans[]);
// I don't know how to name it. This function does shortest path taking into account of passing more than 1 station
int aobs(LocationID from, LocationID to,int rail_hops,LocationID *path);

//dracula's function
void Drac_whereCanIGo(LocationID *canGo, int *nCanGo, LocationID from, int road, int sea);


#endif
