#ifndef _DATAPROC_H_
#define _DATAPROC_H_

#define BUFFERSIZE 800
#define STRINGLEN 100
#define MAXNETDEG 2000
#define MAXEDGES 10000000

#define MAXLEN 20000

#define XRANGE 1536
#define YRANGE 1536

namespace FastRoute {

// global variables
extern int xGrid, yGrid, numGrids, numNets, vCapacity, hCapacity, vCapacity3D[MAXLAYER], hCapacity3D[MAXLAYER];
extern float vCapacity_lb, hCapacity_lb, vCapacity_ub, hCapacity_ub;
extern int layerOrientation;

extern int enlarge, costheight, ripup_threshold;
extern int MaxDegree;
extern int MinWidth[MAXLAYER], MinSpacing[MAXLAYER], ViaSpacing[MAXLAYER];
extern int xcorner, ycorner, wTile, hTile, ahTH;

extern int numValidNets;  // # nets need to be routed (having pins in different grids)
extern int numLayers;
extern int totalNumSeg;    // total # segments
extern int totalOverflow;  // total # overflow
extern int mazeThreshold;  // the wirelen threshold to do maze routing
extern Net **nets;
extern Edge *h_edges, *v_edges;

extern float d1[YRANGE][XRANGE];
extern float d2[YRANGE][XRANGE];

extern Bool HV[YRANGE][XRANGE];
extern Bool hyperV[YRANGE][XRANGE];
extern Bool hyperH[YRANGE][XRANGE];
extern int corrEdge[YRANGE][XRANGE];
extern int SLOPE;

//coefficient
extern float LB;
extern float UB;
extern int THRESH_M;
extern float LOGIS_COF;
extern int ENLARGE;
extern int STEP;
extern int COSHEIGHT;
extern int STOP;
extern int L;
extern int VCA;
extern int VIA, slope, max_adj;
extern char benchFile[STRINGLEN];

extern Segment *seglist;
extern int *seglistIndex;  // the index for the segments for each net
extern int *seglistCnt;    // the number of segements for each net
extern int *segOrder;      // the order of segments for routing

extern Tree *trees;      // the tree topologies
extern StTree *sttrees;  // the Steiner trees
extern DTYPE **gxs;      // the copy of xs for nets, used for second FLUTE
extern DTYPE **gys;      // the copy of xs for nets, used for second FLUTE
extern DTYPE **gs;       // the copy of vertical sequence for nets, used for second FLUTE

extern OrderNetPin *treeOrderPV;
extern OrderTree *treeOrderCong;
extern int numTreeedges;
extern int viacost;

extern Edge3D *h_edges3D;
extern Edge3D *v_edges3D;

extern int layerGrid[MAXLAYER][MAXLEN];
extern int gridD[MAXLAYER][MAXLEN];
extern int viaLink[MAXLAYER][MAXLEN];

extern int d13D[MAXLAYER][YRANGE][XRANGE];
extern short d23D[MAXLAYER][YRANGE][XRANGE];

extern dirctionT ***directions3D;
extern int ***corrEdge3D;
extern parent3D ***pr3D;

extern int mazeedge_Threshold;
extern Bool inRegion[YRANGE][XRANGE];
extern Bool heapVisited[MAXNETDEG];
extern int heapQueue[MAXNETDEG];

extern int gridHV, gridH, gridV, gridHs[MAXLAYER], gridVs[MAXLAYER];

extern int **heap13D;
extern short **heap23D;

extern float *h_costTable, *v_costTable;

extern Bool stopDEC, errorPRONE;
extern OrderNetEdge netEO[2000];

extern int xcor[2000], ycor[2000], dcor[2000];

extern StTree *sttreesBK;

extern short **parentX1, **parentY1, **parentX3, **parentY3;

extern float **heap2, **heap1;
extern Bool *pop_heap2;

extern void init_usage();
extern void readFile(char benchFile[]);
extern void freeAllMemory();
}  // namespace FastRoute

#endif /* _DATAPROC_H_ */
