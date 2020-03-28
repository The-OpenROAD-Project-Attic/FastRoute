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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DataType.h"
#include "flute.h"
#include "DataProc.h"

namespace FastRoute {

// Global variables
int xGrid, yGrid, numGrids, numNets, *vCapacity3D, *hCapacity3D;
float vCapacity_lb, hCapacity_lb, vCapacity_ub, hCapacity_ub;
int MaxDegree;
int *MinWidth, *MinSpacing, *ViaSpacing;
int xcorner, ycorner, wTile, hTile;
int enlarge, costheight, ripup_threshold, ahTH;
int numValidNets;  // # nets need to be routed (having pins in different grids)
int numLayers;
int totalNumSeg;    // total # segments
int totalOverflow;  // total # overflow
int mazeThreshold;  // the wirelen threshold to do maze routing
Net** nets;
Edge *h_edges, *v_edges;
float d1[YRANGE][XRANGE];
float d2[YRANGE][XRANGE];
int layerOrientation;
bool pdRev;
float alpha;
int verbose;
int overflowIterations;

Bool **HV;
Bool **hyperV;
Bool **hyperH;

int **corrEdge;
int SLOPE;

float LB;
float UB;
int THRESH_M;
float LOGIS_COF;
int ENLARGE;
int STEP;
int COSHEIGHT;
int STOP;
int VCA;
int L;
int VIA, slope, max_adj;

char benchFile[STRINGLEN];

Segment* seglist;
int* seglistIndex;  // the index for the segments for each net
int* seglistCnt;    // the number of segements for each net
int* segOrder;      // the order of segments for routing
Tree* trees;        // the tree topologies
StTree* sttrees;    // the Steiner trees
DTYPE** gxs;        // the copy of xs for nets, used for second FLUTE
DTYPE** gys;        // the copy of xs for nets, used for second FLUTE
DTYPE** gs;         // the copy of vertical sequence for nets, used for second FLUTE
Edge3D* h_edges3D;
Edge3D* v_edges3D;

OrderNetPin* treeOrderPV;
OrderTree* treeOrderCong;
int numTreeedges;
int viacost;

int **layerGrid;
int **gridD;
int **viaLink;

int *d13D;
short *d23D;

dirctionT*** directions3D;
int*** corrEdge3D;
parent3D*** pr3D;

int mazeedge_Threshold;
Bool **inRegion;

int gridHV, gridH, gridV, *gridHs, *gridVs;

int** heap13D;
short** heap23D;

float *h_costTable, *v_costTable;
Bool stopDEC, errorPRONE;
OrderNetEdge *netEO;
int *xcor, *ycor, *dcor;

StTree* sttreesBK;

short **parentX1, **parentY1, **parentX3, **parentY3;

float **heap2, **heap1;

Bool* pop_heap2;

void init_usage() {
        int i;

        for (i = 0; i < yGrid * (xGrid - 1); i++)
                h_edges[i].usage = 0;
        for (i = 0; i < (yGrid - 1) * xGrid; i++)
                v_edges[i].usage = 0;
}

void freeAllMemory() {
        int i, deg, numEdges, edgeID;
        TreeEdge* treeedge;

        for (i = 0; i < numValidNets; i++) {
                free(nets[i]->pinX);
                free(nets[i]->pinY);
                free(nets[i]->pinL);
                free(nets[i]);
        }
        free(seglistIndex);
        free(seglistCnt);
        free(seglist);
        free(h_edges);
        free(v_edges);
        free(h_edges3D);
        free(v_edges3D);
        free(segOrder);

        for (i = 0; i < numValidNets; i++)
                free(trees[i].branch);
        free(trees);

        for (i = 0; i < numValidNets; i++) {
                deg = sttrees[i].deg;
                numEdges = 2 * deg - 3;
                for (edgeID = 0; edgeID < numEdges; edgeID++) {
                        treeedge = &(sttrees[i].edges[edgeID]);
                        if (treeedge->len > 0) {
                                free(treeedge->route.gridsX);
                                free(treeedge->route.gridsY);
                                free(treeedge->route.gridsL);
                        }
                }
                free(sttrees[i].nodes);
                free(sttrees[i].edges);
        }
        free(sttrees);

        for (i = 0; i < yGrid; i++) {
                free(parentX1[i]);
                free(parentY1[i]);
                free(parentX3[i]);
                free(parentY3[i]);
        }
        free(parentX1);
        free(parentY1);
        free(parentX3);
        free(parentY3);
        free(pop_heap2);
        free(heap1);
        free(heap2);
}
}  // namespace FastRoute
