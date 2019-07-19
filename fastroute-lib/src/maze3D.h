#ifndef __MAZE3D_H__
#define __MAZE3D_H__

namespace FastRoute {
typedef struct
{
        int x;  // x position
        int y;  // y position
        int l;
} Pos3D;

void mazeRouteMSMDOrder3D(int expand, int ripupTHlb, int ripupTHub);

}  // namespace FastRoute
#endif /* __MAZE3D_H__ */
