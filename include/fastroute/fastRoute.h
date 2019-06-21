#ifndef FASTROUTE_H
#define FASTROUTE_H
#include <vector>

namespace FastRoute {

	typedef struct{
		long x;
		long y;
		int layer;
	} PIN;
	
	typedef struct{
		long initX;
		long initY;
		int initLayer;
		long finalX;
		long finalY;
		int finalLayer;
	} ROUTE;

	typedef struct{
		char* name;
		int id;
		std::vector<ROUTE> route;
	} NET;


	extern void setGridsAndLayers(int x, int y, int nLayers);
	extern void addVCapacity(int verticalCapacity, int layer);
	extern void addHCapacity(int horizontalCapacity, int layer);
	extern void addMinWidth(int width, int layer);
	extern void addMinSpacing(int spacing, int layer);
	extern void addViaSpacing(int spacing, int layer);
	extern void setNumberNets(int nNets);
	extern void setLowerLeft(int x, int y);
	extern void setTileSize(int width, int height);
	extern void addNet(char * name, int netIdx, int nPins, int minWidth, PIN pins[]);
	extern void initEdges();
	extern void setNumAdjustments(int nAdjustments);
	extern void addAdjustment(long x1, long y1, int l1, long x2, long y2, int l2, int reducedCap);
	extern void initAuxVar();
	extern std::vector<NET> getResults();
}
#endif
