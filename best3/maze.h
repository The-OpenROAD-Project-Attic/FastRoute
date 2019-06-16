#ifndef _MAZE_H_
#define _MAZE_H_


typedef struct
{
    int x;   // x position
    int y;   // y position
} Pos;

// Maze-routing in different orders
extern void mazeRouteMSMD(int expand, float height, int ripup_threshold, int mazeedge_Threshold, Bool ordering,int cost_type);
// Maze-routing for multi-source, multi-destination
extern void convertToMazeroute();

extern void updateCongestionHistory(  int round , int upType);
extern void initialCongestionHistory(  int round);
extern void reduceCongestionHistory( int round );

extern int getOverflow2D(  int* maxOverflow);
extern int getOverflow2Dmaze(  int* maxOverflow, int* tUsage);
extern int getOverflow3D(void);

extern void str_accu(int rnd);

extern void InitLastUsage(int upType);
extern void InitEstUsage();




#endif /* _MAZE_H_ */
