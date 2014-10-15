#include <stdio.h>
#include <stdlib.h>
#include "Map.h"
static int effiBFS(LocationID curr,int rail_hops,array,LocationID parent,LocationID visited);
int abs(from,to,rail_hops,path);

int main (int argc,char * argv[]){

	return EXIT_SUCCESS;
}

int abs(from,to,rail_hops,path)
{
    int i = 0;
    int j = 0;
	LocationID visited[NUM_MAP_LOCATIONS] = {0};
	LocationID parent[NUM_MAP_LOCATIONS];
	for (i = 0; i < NUM_MAP_LOCATIONS; i++) parent[i] = -1;
    Map m = newMap();
	VList curr = m->connections[from];
	
	visited[from] = 1;
    int radius = 0;
	LocationID location[NUM_MAP_LOCATIONS][NUM_MAP_LOCATIONS];
	for (i = 0; i < NUM_MAP_LOCATIONS; i++){
		for (j = 0; j < NUM_MAP_LOCATIONS; j++){
			location[i][j] = -1;
		}
	}
	int currnum = 1;
	int nextnum = 0;
    while (!insideArray(to,location[radius])){
    	   i = 0;
    	   nextnum = 0;
		   while (i < currnum) {
		   	   nextnum += effiBFS(location[radius][i],rail_hops,location[radius+1]+nextnum,parent,visited);
		   }
		   currnum = nextnum;
		   radius++;		
	}
	

	int nLocations;
   	int currv = end;
   	for (nLocations = 0; parent[currv] != -1; currv = parent[currv]){
       nLocations++;
   	}
  
    int nOfStations = nLocations+1;
    int par;//parent
    currv = end;
    while (parent[currv] != -1) {
       path[nLocations] = currv;
       par = parent[currv];
       currv = par;
       nLocations--;
    }
    path[0] = currv;
    disposeMap(m);
    return nOfStations;
}














static int effiBFS(LocationID curr,int rail_hops,array,LocationID parent,LocationID visited)
{
    int *numa;
    int *numb;
    int *numc;
    int j;

    LocationID exception[NUM_MAP_LOCATIONS] = {0};    
    
    LocationID *a = conditionalvertex(exception,numa,curr,0,1,0,0);
    LocationID *b = conditionalvertex(exception,numb,curr,0,0,0,1);
    LocationID *c = conditionalvertex(exception,numc,curr,rail_hops,0,1,0);  
    for (j = 0; j <= numa; j++){
        if  (!visited[j]){
       	    array[j] = a[(*numa)--];
       	    visited[j] = 1;
       	    parent[array[j]] = curr;
        }	else{
        	j--;
        }
    }
    for (; j <= num; j++){
        if  (!visited[j]){
       	    array[j] = a[(*numb)--];
       	    visited[j] = 1;
       	    parent[array[j]] = curr;
        }	else{
        	j--;
        }
    }
    for (; j <= num; j++){
        if  (!visited[j]){
       	    array[j] = a[(*numb)--];
       	    visited[j] = 1;
       	    parent[array[j]] = curr;
        }	else{
        	j--;
        }
    }
    int numlocations =  j
    free(a);
    free(b);
    free(c);
    return numLocations;
}
