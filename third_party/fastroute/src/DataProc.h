////////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (c) 2018, Iowa State University All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software
// without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////

#ifndef __DATAPROC_H__
#define __DATAPROC_H__

#define BUFFERSIZE 800
#define STRINGLEN 100
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

extern int gridHV, gridH, gridV, gridHs[MAXLAYER], gridVs[MAXLAYER];

extern int **heap13D;
extern short **heap23D;

extern float *h_costTable, *v_costTable;

extern Bool stopDEC, errorPRONE;
extern OrderNetEdge *netEO;

extern int *xcor, *ycor, *dcor;

extern StTree *sttreesBK;

extern short **parentX1, **parentY1, **parentX3, **parentY3;

extern float **heap2, **heap1;
extern Bool *pop_heap2;

extern void init_usage();
extern void readFile(char benchFile[]);
extern void freeAllMemory();
}  // namespace FastRoute

#endif /* __DATAPROC_H__ */
