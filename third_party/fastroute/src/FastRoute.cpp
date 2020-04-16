////////////////////////////////////////////////////////////////////////////////
// Authors: Vitor Bandeira, Eder Matheus Monteiro e Isadora Oliveira
//          (Advisor: Ricardo Reis)
//
// BSD 3-Clause License
//
// Copyright (c) 2019, Federal University of Rio Grande do Sul (UFRGS)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <algorithm>
#include "DataType.h"
#include "flute.h"
#include "pdrev.h"
#include "DataProc.h"
#include "FastRoute.h"
#include "utility.h"
#include "RSMT.h"
#include "maze.h"
#include "RipUp.h"
#include "route.h"
#include "maze3D.h"
#include <iostream>
#include <string>

namespace FastRoute {

int newnetID;
int segcount;
int pinInd;
int numAdjust;
int vCapacity;
int hCapacity;
int MD;

FT::FT() {
        newnetID = 0;
        segcount = 0;
        pinInd = 0;
        numAdjust = 0;
        vCapacity = 0;
        hCapacity = 0;
        MD = 0;
}

FT::~FT() {
        int i, deg, numEdges, edgeID;
        TreeEdge* treeedge;

        for (i = 0; i < numNets; i++) {
                delete[] nets[i]->pinX;
                delete[] nets[i]->pinY;
                delete[] nets[i]->pinL;
                delete nets[i];
        }

        // for (i = 0; i < numNets; i++) {
        //         delete nets[i];
        // }

        delete[] nets;

        delete[] h_edges;
        delete[] v_edges;

        delete[] seglist;
        delete[] seglistIndex;
        delete[] seglistCnt;

        delete[] gxs;
        delete[] gys;
        delete[] gs;

        if (treeOrderPV != NULL) {
                delete[] treeOrderPV;
                treeOrderPV = NULL;
        }

        if (treeOrderCong != NULL) {
                delete[] treeOrderCong;
                treeOrderCong = NULL;
        }

        delete[] h_edges3D;
        delete[] v_edges3D;

        // for (i = 0; i < numValidNets; i++) {
        //         if (trees[i].branch != NULL) {
        //                 delete[] trees[i].branch;
        //         }
        // }
        delete[] trees;

        for (i = 0; i < numValidNets; i++) {
                deg = sttrees[i].deg;
                numEdges = 2 * deg - 3;
                for (edgeID = 0; edgeID < numEdges; edgeID++) {
                        treeedge = &(sttrees[i].edges[edgeID]);
                        if (treeedge->len > 0) {
                                delete[] treeedge->route.gridsX;
                                delete[] treeedge->route.gridsY;
                                delete[] treeedge->route.gridsL;
                        }
                }
                delete[] sttrees[i].nodes;
                delete[] sttrees[i].edges;
        }
        delete[] sttrees;

        for (i = 0; i < yGrid; i++) {
                delete[] parentX1[i];
                delete[] parentY1[i];
                delete[] parentX3[i];
                delete[] parentY3[i];
        }
        delete[] parentX1;
        delete[] parentY1;
        delete[] parentX3;
        delete[] parentY3;
        delete[] pop_heap2;
        delete[] heap1;
        delete[] heap2;

        delete[] xcor;
        delete[] ycor;
        delete[] dcor;
        delete[] netEO;

        for (int i = 0; i < yGrid; i++) {
                delete[] HV[i];
        }
        delete[] HV;
        
        for (int i = 0; i < yGrid; i++) {
                delete[] hyperV[i];
        }
        delete[] hyperV;
        
        for (int i = 0; i < yGrid; i++) {
                delete[] hyperH[i];
        }
        delete[] hyperH;
        
        for (int i = 0; i < yGrid; i++) {
                delete[] inRegion[i];
        }
        delete[] inRegion;
        
        for (int i = 0; i < yGrid; i++) {
                delete[] corrEdge[i];
        }
        delete[] corrEdge;
        
        delete[] d13D;
        delete[] d23D;
        
        delete[] d1;
        delete[] d2;
        
        delete[] vCapacity3D;
        delete[] hCapacity3D;
        
        delete[] MinWidth;
        delete[] MinSpacing;
        delete[] ViaSpacing;
        
        delete[] gridHs;
        delete[] gridVs;
        
        for (int i = 0; i < numLayers; i++) {
                delete[] layerGrid[i];
        }
        delete[] layerGrid;
        
        for (int i = 0; i < numLayers; i++) {
                delete[] gridD[i];
        }
        delete[] gridD;
        
        for (int i = 0; i < numLayers; i++) {
                delete[] viaLink[i];
        }
        delete[] viaLink;
        
        delete[] costHVH;
        delete[] costVHV;
        delete[] costH;
        delete[] costV;
        delete[] costLR;
        delete[] costTB;

        delete[] costHVHtest;
        delete[] costVtest;
        delete[] costTBtest;
}

void FT::setGridsAndLayers(int x, int y, int nLayers) {
        xGrid = x;
        yGrid = y;
        std::cout << "Num layers: " << nLayers << "\n";
        numLayers = nLayers;
        numGrids = xGrid * yGrid;
        if (std::max(xGrid, yGrid) >= 1000) {
                XRANGE = std::max(xGrid, yGrid);
                YRANGE = std::max(xGrid, yGrid);
        } else {
                XRANGE = 1000;
                YRANGE = 1000;
        }

        vCapacity3D = new int[numLayers];
        hCapacity3D = new int[numLayers];

        for (int i = 0; i < numLayers; i++) {
                vCapacity3D[i] = 0;
                hCapacity3D[i] = 0;
        }

        MinWidth = new int[numLayers];
        MinSpacing = new int[numLayers];
        ViaSpacing = new int[numLayers];
        gridHs = new int[numLayers];
        gridVs = new int[numLayers];
        
        layerGrid = new int*[numLayers];
        for (int i = 0; i < numLayers; i++) {
                layerGrid[i] = new int[MAXLEN];
        }
        
        gridD = new int*[numLayers];
        for (int i = 0; i < numLayers; i++) {
                gridD[i] = new int[MAXLEN];
        }
        
        viaLink = new int*[numLayers];
        for (int i = 0; i < numLayers; i++) {
                viaLink[i] = new int[MAXLEN];
        }

        d13D = new int[numLayers*YRANGE*XRANGE];
        d23D = new short[numLayers*YRANGE*XRANGE];

        d1 = new float[YRANGE * XRANGE];
        d2 = new float[YRANGE * XRANGE];

        HV = new Bool*[YRANGE];
        for (int i = 0; i < YRANGE; i++) {
                HV[i] = new Bool[XRANGE];
        }
        
        hyperV = new Bool*[YRANGE];
        for (int i = 0; i < YRANGE; i++) {
                hyperV[i] = new Bool[XRANGE];
        }
        
        hyperH = new Bool*[YRANGE];
        for (int i = 0; i < YRANGE; i++) {
                hyperH[i] = new Bool[XRANGE];
        }

        corrEdge = new int*[YRANGE];
        for (int i = 0; i < YRANGE; i++) {
                corrEdge[i] = new int[XRANGE];
        }

        inRegion = new Bool*[YRANGE];
        for (int i = 0; i < YRANGE; i++) {
                inRegion[i] = new Bool[XRANGE];
        }

        costHVH = new float[XRANGE];  // Horizontal first Z
        costVHV = new float[YRANGE];  // Vertical first Z
        costH = new float[YRANGE];    // Horizontal segment cost
        costV = new float[XRANGE];    // Vertical segment cost
        costLR = new float[YRANGE];   // Left and right boundary cost
        costTB = new float[XRANGE];   // Top and bottom boundary cost

        costHVHtest = new float[YRANGE];  // Vertical first Z
        costVtest = new float[XRANGE];    // Vertical segment cost
        costTBtest = new float[XRANGE];   // Top and bottom boundary cost
}

void FT::addVCapacity(int verticalCapacity, int layer) {
        vCapacity3D[layer - 1] = verticalCapacity;
        vCapacity += vCapacity3D[layer - 1];
}

void FT::addHCapacity(int horizontalCapacity, int layer) {
        hCapacity3D[layer - 1] = horizontalCapacity;
        hCapacity += hCapacity3D[layer - 1];
}

void FT::addMinWidth(int width, int layer) {
        MinWidth[layer - 1] = width;
}

void FT::addMinSpacing(int spacing, int layer) {
        MinSpacing[layer - 1] = spacing;
}

void FT::addViaSpacing(int spacing, int layer) {
        ViaSpacing[layer - 1] = spacing;
}

void FT::setNumberNets(int nNets) {
        numNets = nNets;
        nets = new Net*[numNets];
        for (int i = 0; i < numNets; i++)
                nets[i] = new Net;
        seglistIndex = new int[numNets];
}

void FT::setLowerLeft(int x, int y) {
        xcorner = x;
        ycorner = y;
}

void FT::setTileSize(int width, int height) {
        wTile = width;
        hTile = height;
}

void FT::setLayerOrientation(int x) {
        layerOrientation = x;
}

void FT::addNet(char *name, int netIdx, int nPins, int minWidth, PIN pins[], float alpha) {
        // std::cout << "Adding net " << name << "\n";
        int TD;
        int i, j, k;
        int pinX, pinY, pinL, netID, numPins, minwidth;
        long pinX_in, pinY_in;
        Bool remove;

        // TODO: check this size
        int pinXarray[nPins];
        int pinYarray[nPins];
        int pinLarray[nPins];

        netID = netIdx;
        numPins = nPins;
        minwidth = minWidth;

        std::string netName(name);
        std::vector<PIN> netPins;
        for (int p = 0; p < numPins; p++) {
                netPins.push_back(pins[p]);
        }

        allNets[netName] = netPins;

        // TODO: check this, there was an if pinInd < 2000
        pinInd = 0;
        for (j = 0; j < numPins; j++) {
                pinX_in = pins[j].x;
                pinY_in = pins[j].y;
                pinL = pins[j].layer;
                pinX = (int)((pinX_in - xcorner) / wTile);
                pinY = (int)((pinY_in - ycorner) / hTile);
                if (!(pinX < 0 || pinX >= xGrid || pinY < -1 || pinY >= yGrid || pinL > numLayers || pinL <= 0)) {
                        remove = FALSE;
                        for (k = 0; k < pinInd; k++) {
                                if (pinX == pinXarray[k] && pinY == pinYarray[k] && pinL == pinLarray[k]) {
                                        remove = TRUE;
                                        break;
                                }
                        }
                        if (!remove)  // the pin is in different grid from other pins
                        {
                                pinXarray[pinInd] = pinX;
                                pinYarray[pinInd] = pinY;
                                pinLarray[pinInd] = pinL;
                                pinInd++;
                        }
                }
        }
        if (pinInd > 1)  // valid net
        {
                MD = std::max(MD, pinInd);
                TD += pinInd;
                // std::cout << "Net name: " << nets[newnetID]->name << "; num pins: " << nets[newnetID]->numPins << "\n";
                strcpy(nets[newnetID]->name, name);
                nets[newnetID]->netIDorg = netID;
                nets[newnetID]->numPins = numPins;
                nets[newnetID]->deg = pinInd;
                nets[newnetID]->pinX = new short[pinInd];
                nets[newnetID]->pinY = new short[pinInd];
                nets[newnetID]->pinL = new short[pinInd];
                nets[newnetID]->alpha = alpha;

                for (j = 0; j < pinInd; j++) {
                        nets[newnetID]->pinX[j] = pinXarray[j];
                        nets[newnetID]->pinY[j] = pinYarray[j];
                        nets[newnetID]->pinL[j] = pinLarray[j];
                }
                seglistIndex[newnetID] = segcount;
                newnetID++;
                segcount += 2 * pinInd - 3;  // at most (2*numPins-2) nodes, (2*numPins-3) nets for a net
        }                                    // if
}

std::map<std::string, std::vector<PIN>> FT::getNets() {
        return allNets;
}

void FT::initEdges() {
        LB = 0.9;
        UB = 1.3;
        int grid, j, k, i;
        int TC;
        vCapacity_lb = LB * vCapacity;
        hCapacity_lb = LB * hCapacity;
        vCapacity_ub = UB * vCapacity;
        hCapacity_ub = UB * hCapacity;

        // TODO: check this, there was an if pinInd > 1 && pinInd < 2000
        if (pinInd > 1) {
                seglistIndex[newnetID] = segcount;  // the end pointer of the seglist
        }
        numValidNets = newnetID;

        // allocate memory and initialize for edges

        h_edges = new Edge[((xGrid - 1) * yGrid)];
        v_edges = new Edge[(xGrid * (yGrid - 1))];

        init_usage();

        v_edges3D = new Edge3D[(numLayers * xGrid * yGrid)];
        h_edges3D = new Edge3D[(numLayers * xGrid * yGrid)];

        //2D edge innitialization
        TC = 0;
        for (i = 0; i < yGrid; i++) {
                for (j = 0; j < xGrid - 1; j++) {
                        grid = i * (xGrid - 1) + j;
                        h_edges[grid].cap = hCapacity;
                        TC += hCapacity;
                        h_edges[grid].usage = 0;
                        h_edges[grid].est_usage = 0;
                        h_edges[grid].red = 0;
                        h_edges[grid].last_usage = 0;
                }
        }
        for (i = 0; i < yGrid - 1; i++) {
                for (j = 0; j < xGrid; j++) {
                        grid = i * xGrid + j;
                        v_edges[grid].cap = vCapacity;
                        TC += vCapacity;
                        v_edges[grid].usage = 0;
                        v_edges[grid].est_usage = 0;
                        v_edges[grid].red = 0;
                        v_edges[grid].last_usage = 0;
                }
        }

        for (k = 0; k < numLayers; k++) {
                for (i = 0; i < yGrid; i++) {
                        for (j = 0; j < xGrid - 1; j++) {
                                grid = i * (xGrid - 1) + j + k * (xGrid - 1) * yGrid;
                                h_edges3D[grid].cap = hCapacity3D[k];
                                h_edges3D[grid].usage = 0;
                                h_edges3D[grid].red = 0;
                        }
                }
                for (i = 0; i < yGrid - 1; i++) {
                        for (j = 0; j < xGrid; j++) {
                                grid = i * xGrid + j + k * xGrid * (yGrid - 1);
                                v_edges3D[grid].cap = vCapacity3D[k];
                                v_edges3D[grid].usage = 0;
                                v_edges3D[grid].red = 0;
                        }
                }
        }
}

void FT::setNumAdjustments(int nAdjustments) {
        numAdjust = nAdjustments;
}

void FT::addAdjustment(long x1, long y1, int l1, long x2, long y2, int l2, int reducedCap, bool isReduce) {
        int grid, k;
        int reduce, cap;
        reducedCap = reducedCap;

        k = l1 - 1;

        if (y1 == y2)  //horizontal edge
        {
                grid = y1 * (xGrid - 1) + x1 + k * (xGrid - 1) * yGrid;
                cap = h_edges3D[grid].cap;

                if (((int)cap - reducedCap) < 0) {
                        if (isReduce) {
                                std::cout << "Warning: underflow in reduce\n";
                                std::cout << "cap, reducedCap: " << cap << ", " << reducedCap << "\n";
                        }
                        reduce = 0;
                } else {
                        reduce = cap - reducedCap;
                }

                h_edges3D[grid].cap = reducedCap;
                h_edges3D[grid].red = reduce;
                grid = y1 * (xGrid - 1) + x1;
                h_edges[grid].cap -= reduce;
                h_edges[grid].red += reduce;

        } else if (x1 == x2)  //vertical edge
        {
                grid = y1 * xGrid + x1 + k * xGrid * (yGrid - 1);
                cap = v_edges3D[grid].cap;
                reduce = cap - reducedCap;

                if (((int)cap - reducedCap) < 0) {
                        if (isReduce) {
                                std::cout << "Warning: underflow in reduce\n";
                                std::cout << "cap, reducedCap: " << cap << ", " << reducedCap << "\n";
                        }
                        reduce = 0;
                } else {
                        reduce = cap - reducedCap;
                }

                v_edges3D[grid].cap = reducedCap;
                v_edges3D[grid].red = reduce;
                grid = y1 * xGrid + x1;
                v_edges[grid].cap -= reduce;
                v_edges[grid].red += reduce;
        }
}

int FT::getEdgeCapacity(long x1, long y1, int l1, long x2, long y2, int l2) {
        int grid, k;
        int cap;

        k = l1 - 1;

        if (y1 == y2)  //horizontal edge
        {
                grid = y1 * (xGrid - 1) + x1 + k * (xGrid - 1) * yGrid;
                cap = h_edges3D[grid].cap;
        } else if (x1 == x2)  //vertical edge
        {
                grid = y1 * xGrid + x1 + k * xGrid * (yGrid - 1);
                cap = v_edges3D[grid].cap;
        }

        return cap;
}

void FT::setMaxNetDegree(int deg) {
        maxNetDegree = deg;
}

void FT::initAuxVar() {
        int k, i;
        treeOrderCong = NULL;
        stopDEC = FALSE;

        seglistCnt = new int[numValidNets];
        seglist = new Segment[segcount];
        trees = new Tree[numValidNets];
        sttrees = new StTree[numValidNets];
        gxs = new DTYPE*[numValidNets];
        gys = new DTYPE*[numValidNets];
        gs = new DTYPE*[numValidNets];

        gridHV = XRANGE * YRANGE;
        gridH = (xGrid - 1) * yGrid;
        gridV = xGrid * (yGrid - 1);
        for (k = 0; k < numLayers; k++) {
                gridHs[k] = k * gridH;
                gridVs[k] = k * gridV;
        }

        MaxDegree = MD;

        parentX1 = new short*[yGrid];
        parentY1 = new short*[yGrid];
        parentX3 = new short*[yGrid];
        parentY3 = new short*[yGrid];

        for (i = 0; i < yGrid; i++) {
                parentX1[i] = new short[xGrid];
                parentY1[i] = new short[xGrid];
                parentX3[i] = new short[xGrid];
                parentY3[i] = new short[xGrid];
        }

        pop_heap2 = new Bool[yGrid * XRANGE];

        // allocate memory for priority queue
        heap1 = new float*[yGrid * xGrid];
        heap2 = new float*[yGrid * xGrid];

        sttreesBK = NULL;
}

std::vector<NET> FT::getResults() {
        short *gridsX, *gridsY, *gridsL;
        int netID, d, i, k, edgeID, nodeID, deg, lastX, lastY, lastL, xreal, yreal, l, routeLen;
        TreeEdge *treeedges, *treeedge;
        TreeNode *nodes;
        TreeEdge edge;
        std::vector<NET> netsOut;
        netsOut.clear();
        for (netID = 0; netID < numValidNets; netID++) {
                NET currentNet;
                std::string netName(nets[netID]->name);
                currentNet.name = netName;
                currentNet.id = netID;
                treeedges = sttrees[netID].edges;
                deg = sttrees[netID].deg;

                nodes = sttrees[netID].nodes;
                for (edgeID = 0; edgeID < 2 * deg - 3; edgeID++) {
                        edge = sttrees[netID].edges[edgeID];
                        treeedge = &(treeedges[edgeID]);
                        if (treeedge->len > 0) {
                                routeLen = treeedge->route.routelen;
                                gridsX = treeedge->route.gridsX;
                                gridsY = treeedge->route.gridsY;
                                gridsL = treeedge->route.gridsL;
                                lastX = wTile * (gridsX[0] + 0.5) + xcorner;
                                lastY = hTile * (gridsY[0] + 0.5) + ycorner;
                                lastL = gridsL[0];
                                for (i = 1; i <= routeLen; i++) {
                                        xreal = wTile * (gridsX[i] + 0.5) + xcorner;
                                        yreal = hTile * (gridsY[i] + 0.5) + ycorner;

                                        ROUTE routing;
                                        routing.initX = lastX;
                                        routing.initY = lastY;
                                        routing.initLayer = lastL + 1;
                                        routing.finalX = xreal;
                                        routing.finalY = yreal;
                                        routing.finalLayer = gridsL[i] + 1;
                                        lastX = xreal;
                                        lastY = yreal;
                                        lastL = gridsL[i];
                                        currentNet.route.push_back(routing);
                                }
                        }
                }
                netsOut.push_back(currentNet);
        }

        return netsOut;
}

int FT::run(std::vector<NET> &result) {
        //    char benchFile[FILESTRLEN];
        char routingFile[STRINGLEN];
        char degreeFile[STRINGLEN];
        char optionS[STRINGLEN];
        clock_t t1 = 0, t2 = 0, t3 = 0, t4 = 0;
        float gen_brk_Time, P1_Time, P2_Time, P3_Time, maze_Time, totalTime, congestionmap_time;
        int iter, last_totalOverflow, diff_totalOverflow, enlarge, ripup_threshold;
        int i, j, past_overflow, cur_overflow;
        int L_afterSTOP;
        int ESTEP1, CSTEP1, thStep1;
        int ESTEP2, CSTEP2, thStep2;
        int ESTEP3, CSTEP3, thStep3, tUsage, CSTEP4;
        int Ripvalue, LVIter, cost_step;
        int maxOverflow, past_cong, last_cong, finallength, numVia, ripupTH3D, newTH, healingTrigger;
        int updateType, minofl, minoflrnd, mazeRound, upType, cost_type, bmfl, bwcnt;

        Bool goingLV, healingNeed, noADJ, extremeNeeded;

        // TODO: check this size
        int maxPin = maxNetDegree;
        maxPin = 2* maxPin;
        xcor = new int[maxPin];
        ycor = new int[maxPin];
        dcor = new int[maxPin];
        netEO = new OrderNetEdge[maxPin];


        Bool input, WriteOut;
        input = WriteOut = 0;

        LB = 0.9;
        UB = 1.3;

        SLOPE = 5;
        THRESH_M = 20;
        ENLARGE = 15;  //5
        ESTEP1 = 10;   //10
        ESTEP2 = 5;    //5
        ESTEP3 = 5;    //5
        CSTEP1 = 2;    //5
        CSTEP2 = 2;    //3
        CSTEP3 = 5;    //15
        CSTEP4 = 1000;
        COSHEIGHT = 4;
        L = 0;
        VIA = 2;
        L_afterSTOP = 1;
        Ripvalue = -1;
        ripupTH3D = 10;
        goingLV = TRUE;
        noADJ = FALSE;
        thStep1 = 10;
        thStep2 = 4;
        healingNeed = FALSE;
        updateType = 0;
        LVIter = 3;
        extremeNeeded = FALSE;
        mazeRound = 500;
        bmfl = BIG_INT;
        minofl = BIG_INT;

        // call FLUTE to generate RSMT and break the nets into segments (2-pin nets)

        t1 = clock();

        VIA = 2;
        //viacost = VIA;
        viacost = 0;
        gen_brk_RSMT(FALSE, FALSE, FALSE, FALSE, noADJ);
        if (verbose > 1)
                printf(" > --first L\n");
        routeLAll(TRUE);
        gen_brk_RSMT(TRUE, TRUE, TRUE, FALSE, noADJ);
        getOverflow2D(&maxOverflow);
        if (verbose > 1)
                printf(" > --second L\n");
        newrouteLAll(FALSE, TRUE);
        getOverflow2D(&maxOverflow);
        spiralRouteAll();
        newrouteZAll(10);
        if (verbose > 1)
                printf(" > --first Z\n");
        past_cong = getOverflow2D(&maxOverflow);

        convertToMazeroute();

        enlarge = 10;
        newTH = 10;
        healingTrigger = 0;
        stopDEC = 0;
        upType = 1;
        //iniBDE();
        costheight = COSHEIGHT;
        if (maxOverflow > 700) {
                costheight = 8;
                LOGIS_COF = 1.33;
                VIA = 0;
                THRESH_M = 0;
                CSTEP1 = 30;
                slope = BIG_INT;
        }

        for (i = 0; i < LVIter; i++) {
                LOGIS_COF = std::max<float>(2.0 / (1 + log(maxOverflow)), LOGIS_COF);
                LOGIS_COF = 2.0 / (1 + log(maxOverflow));
                if (verbose > 1)
                        printf(" > --LV routing round %d, enlarge %d \n", i, enlarge);
                routeLVAll(newTH, enlarge);

                past_cong = getOverflow2Dmaze(&maxOverflow, &tUsage);

                enlarge += 5;
                newTH -= 5;
                if (newTH < 1) {
                        newTH = 1;
                }
        }

        //	past_cong = getOverflow2Dmaze( &maxOverflow);

        t3 = clock();
        InitEstUsage();

        i = 1;
        costheight = COSHEIGHT;
        enlarge = ENLARGE;
        ripup_threshold = Ripvalue;

        minofl = totalOverflow;
        stopDEC = FALSE;

        slope = 20;
        L = 1;
        cost_type = 1;

        InitLastUsage(upType);
        if (totalOverflow > 0) {
                printf(" > --Running extra iterations to remove overflow...\n");
        }
        
        while (totalOverflow > 0 && i <= overflowIterations) {
                if (THRESH_M > 15) {
                        THRESH_M -= thStep1;
                } else if (THRESH_M >= 2) {
                        THRESH_M -= thStep2;
                } else {
                        THRESH_M = 0;
                }
                if (THRESH_M <= 0) {
                        THRESH_M = 0;
                }

                if (totalOverflow > 2000) {
                        enlarge += ESTEP1;  //ENLARGE+(i-1)*ESTEP;
                        cost_step = CSTEP1;
                        updateCongestionHistory(i, upType);
                } else if (totalOverflow < 500) {
                        cost_step = CSTEP3;
                        enlarge += ESTEP3;
                        ripup_threshold = -1;
                        updateCongestionHistory(i, upType);
                } else {
                        cost_step = CSTEP2;
                        enlarge += ESTEP2;
                        updateCongestionHistory(i, upType);
                }

                if (totalOverflow > 15000 && maxOverflow > 400) {
                        enlarge = std::max(xGrid, yGrid) / 30;
                        slope = BIG_INT;
                        if (i == 5) {
                                VIA = 0;
                                LOGIS_COF = 1.33;
                                ripup_threshold = -1;
                                //	cost_type = 3;

                        } else if (i > 6) {
                                if (i % 2 == 0) {
                                        LOGIS_COF += 0.5;
                                }
                                if (i > 40) {
                                        break;
                                }
                        }
                        if (i > 10) {
                                cost_type = 1;
                                ripup_threshold = 0;
                        }
                }

                enlarge = std::min(enlarge, xGrid / 2);
                costheight += cost_step;
                mazeedge_Threshold = THRESH_M;

                if (upType == 3) {
                        LOGIS_COF = std::max<float>(2.0 / (1 + log(maxOverflow + max_adj)), LOGIS_COF);
                } else {
                        LOGIS_COF = std::max<float>(2.0 / (1 + log(maxOverflow)), LOGIS_COF);
                }

                if (i == 8) {
                        L = 0;
                        upType = 2;
                        InitLastUsage(upType);
                }

                if (maxOverflow == 1) {
                        //L = 0;
                        ripup_threshold = -1;
                        slope = 5;
                }

                if (maxOverflow > 300 && past_cong > 15000) {
                        L = 0;
                }

                printf(" > ----iteration %d, enlarge %d, costheight %d, threshold %d via cost %d \n > ----log_coef %f, healingTrigger %d cost_step %d L %d cost_type %d updatetype %d\n", i, enlarge, costheight, mazeedge_Threshold, VIA, LOGIS_COF, healingTrigger, cost_step, L, cost_type, upType);
                mazeRouteMSMD(i, enlarge, costheight, ripup_threshold, mazeedge_Threshold, !(i % 3), cost_type);
                last_cong = past_cong;
                past_cong = getOverflow2Dmaze(&maxOverflow, &tUsage);

                if (minofl > past_cong) {
                        minofl = past_cong;
                        minoflrnd = i;
                }

                if (i == 8) {
                        L = 1;
                }

                i++;

                if (past_cong < 200 && i > 30 && upType == 2 && max_adj <= 20) {
                        upType = 4;
                        stopDEC = TRUE;
                }

                if (maxOverflow < 150) {
                        if (i == 20 && past_cong > 200) {
                                printf(" > ----Extra Run for hard benchmark\n");
                                L = 0;
                                upType = 3;
                                stopDEC = TRUE;
                                slope = 5;
                                mazeRouteMSMD(i, enlarge, costheight, ripup_threshold, mazeedge_Threshold, !(i % 3), cost_type);
                                last_cong = past_cong;
                                past_cong = getOverflow2Dmaze(&maxOverflow, &tUsage);

                                str_accu(12);
                                L = 1;
                                stopDEC = FALSE;
                                slope = 3;
                                upType = 2;
                        }
                        if (i == 35 && tUsage > 800000) {
                                str_accu(25);
                        }
                        if (i == 50 && tUsage > 800000) {
                                str_accu(40);
                        }
                }

                if (i > 50) {
                        upType = 4;
                        if (i > 70) {
                                stopDEC = TRUE;
                        }
                }

                if (past_cong > 0.7 * last_cong) {
                        costheight += CSTEP3;
                }

                if (past_cong >= last_cong) {
                        VIA = 0;
                        healingTrigger++;
                }

                if (past_cong < bmfl) {
                        bwcnt = 0;
                        if (i > 140 || (i > 80 && past_cong < 20)) {
                                copyRS();
                                bmfl = past_cong;

                                L = 0;
                                SLOPE = BIG_INT;
                                mazeRouteMSMD(i, enlarge, costheight, ripup_threshold, mazeedge_Threshold, !(i % 3), cost_type);
                                last_cong = past_cong;
                                past_cong = getOverflow2Dmaze(&maxOverflow, &tUsage);
                                if (past_cong < last_cong) {
                                        copyRS();
                                        bmfl = past_cong;
                                }
                                L = 1;
                                SLOPE = 5;
                                if (minofl > past_cong) {
                                        minofl = past_cong;
                                        minoflrnd = i;
                                }
                        }
                } else {
                        bwcnt++;
                }

                if (bmfl > 10) {
                        if (bmfl > 30 && bmfl < 72 && bwcnt > 50) {
                                break;
                        }
                        if (bmfl < 30 && bwcnt > 50) {
                                break;
                        }
                        if (i >= mazeRound) {
                                getOverflow2Dmaze(&maxOverflow, &tUsage);
                                break;
                        }
                }

                if (i >= mazeRound) {
                        getOverflow2Dmaze(&maxOverflow, &tUsage);
                        break;
                }
        }
        
        if (totalOverflow > 0 && !allowOverflow) {
                printf("[ERROR] FastRoute cannot handle very congested design\n");
                std::exit(2);
        }

        if (allowOverflow && totalOverflow > 0) {
                printf("[WARNING] Global routing finished with overflow!");
        }

        if (minofl > 0) {
                printf("\n\n minimal ofl %d, occuring at round %d\n\n", minofl, minoflrnd);
                copyBR();
        }

        freeRR();

        checkUsage();

        if (verbose > 1)
            printf(" > --maze routing finished\n");

        t4 = clock();
        maze_Time = (float)(t4 - t3) / CLOCKS_PER_SEC;
        
        if (verbose > 1) {
            printf(" > --P3 runtime: %f sec\n", maze_Time);

            printf(" > --Final 2D results: \n");
        }
        getOverflow2Dmaze(&maxOverflow, &tUsage);

        if (verbose > 1)
                printf(" > \n > --Layer Assignment Begins\n");
        newLA();
        if (verbose > 1)
                printf(" > --layer assignment finished\n");

        t2 = clock();
        gen_brk_Time = (float)(t2 - t1) / CLOCKS_PER_SEC;
        
        if (verbose > 1)
            printf(" > --2D + Layer Assignment Runtime: %f sec\n", gen_brk_Time);

        costheight = 3;
        viacost = 1;

        if (gen_brk_Time < 60) {
                ripupTH3D = 15;
        } else if (gen_brk_Time < 120) {
                ripupTH3D = 18;
        } else {
                ripupTH3D = 20;
        }

        if (goingLV && past_cong == 0) {
                if (verbose > 1)
                    printf(" > --Post Processing Begins \n");
                mazeRouteMSMDOrder3D(enlarge, 0, ripupTH3D);

                //	mazeRouteMSMDOrder3D(enlarge, 0, 10 );
                if (gen_brk_Time > 120) {
                        mazeRouteMSMDOrder3D(enlarge, 0, 12);
                }
                if (verbose > 1)
                        printf(" > --Post Processsing finished, starting via filling\n");
        }

        fillVIA();
        finallength = getOverflow3D();
        numVia = threeDVIA();
        checkRoute3D();

        t4 = clock();
        maze_Time = (float)(t4 - t1) / CLOCKS_PER_SEC;
        printf(" > --Final usage          : %d\n", finallength);
        printf(" > --Final number of vias : %d\n", numVia);
        printf(" > --Final usage 3D       : %d\n", (finallength + 3 * numVia));

        std::cout << " > --Getting results...\n";
        result = getResults();
        std::cout << " > --Getting results... Done!\n";
        std::cout << " > \n";

        /* TODO:  <11-07-19, this function leads to a segfault, but as the OS
         * frees all memory after the application end (next line) we can omit
         * this function call for now.> */
        /* freeAllMemory(); */
        return (0);
}

void FT::usePdRev(){
        pdRev = true;
}

void FT::setAlpha(float a){
        alpha = a;
}

void FT::setVerbose(int v){
        verbose = v;
}

void FT::setOverflowIterations(int iterations){
        overflowIterations = iterations;
}

void FT::setPDRevForHighFanout(int pdRevHihgFanout){
        pdRevForHighFanout = pdRevHihgFanout;
}

void FT::setAllowOverflow(bool allow) {
        allowOverflow = allow;
}

}  // namespace FastRoute
