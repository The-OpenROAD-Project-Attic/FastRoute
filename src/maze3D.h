#ifndef _MAZE3D_H_
#define _MAZE3D_H_

namespace FastRoute{
typedef struct
{
    int x;   // x position
    int y;   // y position
	int l;
} Pos3D;

void mazeRouteMSMDOrder3D(int expand, int ripupTHlb, int ripupTHub);

}
#endif /* _MAZE3D_H_ */
