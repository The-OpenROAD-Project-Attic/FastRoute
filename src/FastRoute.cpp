#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DataType.h"
#include "flute.h"
#include "DataProc.h"
#include "FastRoute.h"
#include "utility.h"

namespace FastRoute{

int newnetID =0;
int segcount = 0;
int pinInd;
int numAdjust;
int vCapacity = 0;
int  hCapacity = 0;
int MD =0;

void setGridsAndLayers(int x, int y, int nLayers){
	xGrid = x;
	yGrid = y;
	numLayers = nLayers;
        numGrids = xGrid*yGrid;
}

void addVCapacity(int verticalCapacity, int layer){
	vCapacity3D[layer-1]=verticalCapacity/2;
	vCapacity +=vCapacity3D[layer-1];	
}

void addHCapacity(int horizontalCapacity, int layer){
	hCapacity3D[layer-1]=horizontalCapacity/2;
	hCapacity +=hCapacity3D[layer-1];	
}

void addMinWidth(int width, int layer){
	MinWidth[layer-1]=width;
}

void addMinSpacing(int spacing, int layer){
	MinSpacing[layer-1]=spacing;
}

void addViaSpacing(int spacing, int layer){
	ViaSpacing[layer-1]=spacing;
}

void setNumberNets(int nNets){
	numNets=nNets;
}

void setLowerLeft(int x, int y){
	xcorner = x;
	ycorner = y;
}

void setTileSize(int width, int height){
	wTile = width;
	hTile = height;
}

void addNet(char* name, int netIdx, int nPins, int minWidth, PIN pins[]){
	int TD;
	int i,j,k;
    	int pinX, pinY, pinL, netID, numPins, minwidth;
    	long pinX_in,pinY_in;
    	int maxDeg = 0;
    	int pinXarray[MAXNETDEG], pinYarray[MAXNETDEG], pinLarray[MAXNETDEG];
	Bool remove;
	if (nets == NULL){
		nets = (Net**) malloc(numNets*sizeof(Net*));
    		for(i=0; i<numNets; i++)
        		nets[i] = (Net*) malloc(sizeof(Net));
    		seglistIndex = (int*) malloc(numNets*sizeof(int));

	}
	netID = netIdx;
	numPins = nPins;
	minwidth = minWidth;
	if (numPins<1000){
        pinInd = 0;
        for(j=0; j<numPins; j++)
        {
            pinX_in = pins[j].x;
	    pinY_in = pins[j].y; 
	    pinL = pins[j].layer;
            pinX = (int)((pinX_in-xcorner)/wTile);
            pinY = (int)((pinY_in-ycorner)/hTile);
            if(!(pinX < 0 || pinX >= xGrid || pinY < -1 || pinY >= yGrid|| pinL >=numLayers ||pinL<0))
            {
                remove = FALSE;
                for(k=0; k<pinInd; k++)
                {
                    if(pinX==pinXarray[k] && pinY==pinYarray[k] && pinL==pinLarray[k])
                    {remove=TRUE; break;}
                }
                if(!remove) // the pin is in different grid from other pins
                {
                    pinXarray[pinInd] = pinX;
                    pinYarray[pinInd] = pinY;
                    pinLarray[pinInd] = pinL;
                    pinInd++;
                }
            }
        }
        if(pinInd>1) // valid net
        {
            MD = maxFlute(MD, pinInd);
            TD += pinInd;        
            strcpy(nets[newnetID]->name, name);
            nets[newnetID]->netIDorg = netID;
            nets[newnetID]->numPins = numPins;
            nets[newnetID]->deg = pinInd;
            nets[newnetID]->pinX = (short*) malloc(pinInd*sizeof(short));
            nets[newnetID]->pinY = (short*) malloc(pinInd*sizeof(short));
            nets[newnetID]->pinL = (short*) malloc(pinInd*sizeof(short));

            for (j=0; j<pinInd; j++)
            {
                nets[newnetID]->pinX[j] = pinXarray[j];
                nets[newnetID]->pinY[j] = pinYarray[j];
                nets[newnetID]->pinL[j] = pinLarray[j];
            }
            maxDeg = pinInd>maxDeg?pinInd:maxDeg;
            seglistIndex[newnetID] = segcount;
            newnetID++;
            segcount += 2*pinInd-3; // at most (2*numPins-2) nodes, (2*numPins-3) nets for a net
        } // if
       }//if

       else
       {
         for (j=0;j<numPins;j++){
            pinX_in = pins[j].x;
	    pinY_in = pins[j].y; 
	    pinL = pins[j].layer;
	}
       }
}


void initEdges(){
	LB = 0.9;
	UB = 1.3;
	int grid,j,k,i;
	int TC;
    vCapacity_lb = LB*vCapacity;
    hCapacity_lb = LB*hCapacity;
    vCapacity_ub = UB*vCapacity;
    hCapacity_ub = UB*hCapacity;
 
	if((pinInd>1)&&(pinInd<1000))
    {
		seglistIndex[newnetID] = segcount; // the end pointer of the seglist
    }
    numValidNets = newnetID;
    
       // allocate memory and initialize for edges

    h_edges = (Edge*)calloc(((xGrid-1)*yGrid), sizeof(Edge));
    v_edges = (Edge*)calloc((xGrid*(yGrid-1)), sizeof(Edge));

	v_edges3D = (Edge3D*)calloc((numLayers*xGrid*yGrid), sizeof(Edge3D));
	h_edges3D = (Edge3D*)calloc((numLayers*xGrid*yGrid), sizeof(Edge3D));

    //2D edge innitialization
    TC=0;
    for(i=0; i<yGrid; i++)
    {
        for(j=0; j<xGrid-1; j++)
        {
            grid = i*(xGrid-1)+j;
            h_edges[grid].cap = hCapacity;
            TC+=hCapacity;
            h_edges[grid].usage = 0;
            h_edges[grid].est_usage = 0;
            h_edges[grid].red=0;
			h_edges[grid].last_usage=0;

        }
    }
    for(i=0; i<yGrid-1; i++)
    {
        for(j=0; j<xGrid; j++)
        {
            grid = i*xGrid+j;
            v_edges[grid].cap = vCapacity;
            TC+=vCapacity;
            v_edges[grid].usage = 0;
            v_edges[grid].est_usage = 0;
            v_edges[grid].red=0;
			v_edges[grid].last_usage=0;

        }
    }

	for (k=0; k<numLayers; k++)
	{
		for(i=0; i<yGrid; i++)
		{
			for(j=0; j<xGrid-1; j++)
			{
				grid = i*(xGrid-1)+j+k*(xGrid-1)*yGrid;
				h_edges3D[grid].cap = hCapacity3D[k];
				h_edges3D[grid].usage = 0;
				h_edges3D[grid].red = 0;
				
			} 
		}
		for(i=0; i<yGrid-1; i++)
		{
			for(j=0; j<xGrid; j++)
			{
				grid = i*xGrid+j+k*xGrid*(yGrid-1);
				v_edges3D[grid].cap = vCapacity3D[k];
				v_edges3D[grid].usage = 0;
				v_edges3D[grid].red = 0;
            }
		}
	}

}

void setNumAdjustments(int nAdjustments){
	numAdjust = nAdjustments;
}

void addAdjustment(long x1, long y1, int l1, long x2, long y2, int l2, int reducedCap){
		int grid,k;
		int reduce,cap;
		reducedCap = reducedCap/2;

		k = l1-1;

		if (y1==y2)//horizontal edge
		{
			grid = y1*(xGrid-1)+x1+k*(xGrid-1)*yGrid;
			cap=h_edges3D[grid].cap;
			reduce=cap-reducedCap;
			h_edges3D[grid].cap=reducedCap;
			h_edges3D[grid].red=reduce;
			grid=y1*(xGrid-1)+x1;
			h_edges[grid].cap-=reduce;
			h_edges[grid].red += reduce;
			

		} else if (x1==x2)//vertical edge
		{
			grid = y1*xGrid+x1+k*xGrid*(yGrid-1);
			cap=v_edges3D[grid].cap;
			reduce=cap-reducedCap;
			v_edges3D[grid].cap=reducedCap;
			v_edges3D[grid].red=reduce;
			grid = y1*xGrid+x1;
			v_edges[grid].cap-=reduce;
			v_edges[grid].red += reduce;
			
		}
}

void initAuxVar(){
	int k,i;
	treeOrderCong = NULL;
	stopDEC = FALSE;

    seglistCnt = (int*) malloc(numValidNets*sizeof(int));
    seglist = (Segment*) malloc(segcount*sizeof(Segment));
    trees = (Tree*) malloc(numValidNets*sizeof(Tree));
    sttrees = (StTree*) malloc(numValidNets*sizeof(StTree));
    gxs = (DTYPE**) malloc(numValidNets*sizeof(DTYPE*));
    gys = (DTYPE**) malloc(numValidNets*sizeof(DTYPE*));
    gs =  (DTYPE**) malloc(numValidNets*sizeof(DTYPE*));

	gridHV = XRANGE*YRANGE;
	gridH = (xGrid-1)*yGrid;
	gridV = xGrid*(yGrid-1);
    for (k=0; k<numLayers; k++)
	{
		gridHs[k] = k*gridH;
		gridVs[k] = k * gridV;
	}
    
    MaxDegree=MD;


	parentX1 = (short**)calloc(yGrid, sizeof(short*));
    parentY1 = (short**)calloc(yGrid, sizeof(short*));
    parentX3 = (short**)calloc(yGrid, sizeof(short*));
    parentY3 = (short**)calloc(yGrid, sizeof(short*));

   
    for(i=0; i<yGrid; i++)
    {
        parentX1[i] = (short*)calloc(xGrid, sizeof(short));
        parentY1[i] = (short*)calloc(xGrid, sizeof(short));
        parentX3[i] = (short*)calloc(xGrid, sizeof(short));
        parentY3[i] = (short*)calloc(xGrid, sizeof(short));
    }
    
    pop_heap2 = (Bool*)calloc(yGrid*XRANGE, sizeof(Bool));

    // allocate memory for priority queue
    heap1 = (float**)calloc((yGrid*xGrid), sizeof(float*));
    heap2 = (float**)calloc((yGrid*xGrid), sizeof(float*));



	sttreesBK = NULL;
	
}

std::vector<NET> getResults(){
	short *gridsX, *gridsY, *gridsL;
	int netID, d, i,k, edgeID,nodeID,deg, lastX, lastY,lastL, xreal, yreal,l, routeLen;
	TreeEdge *treeedges, *treeedge;
	FILE *fp;
	TreeNode *nodes;
	TreeEdge edge;
	std::vector<NET> netsOut;
	for(netID=0;netID<numValidNets;netID++)
	{
		NET currentNet;
		strcpy(currentNet.name, nets[netID]->name);
		currentNet.id = netID;
		treeedges=sttrees[netID].edges;
		deg=sttrees[netID].deg;
			

		nodes = sttrees[netID].nodes;
		for(edgeID = 0 ; edgeID < 2*deg-3; edgeID++)
		{
			edge = sttrees[netID].edges[edgeID];
			treeedge = &(treeedges[edgeID]);
			if (treeedge->len > 0) {
				
				routeLen = treeedge->route.routelen;
				gridsX = treeedge->route.gridsX;
				gridsY = treeedge->route.gridsY;
				gridsL = treeedge->route.gridsL;
				lastX = wTile*(gridsX[0]+0.5)+xcorner;
				lastY = hTile*(gridsY[0]+0.5)+ycorner;
				lastL = gridsL[0];
				for (i = 1; i <= routeLen; i ++) {
					xreal = wTile*(gridsX[i]+0.5)+xcorner;
					yreal = hTile*(gridsY[i]+0.5)+ycorner;

					ROUTE routing;
					routing.initX = lastX;
					routing.initY - lastY;
					routing.initLayer = lastL+1;
					routing.finalX = xreal;
					routing.finalY = yreal;
					routing.finalLayer = gridsL[i]+1;
					lastX = xreal;
					lastY = yreal;
					lastL = gridsL[i];
					currentNet.route.push_back(routing);
				}
			}
		}
		netsOut.push_back(currentNet);
	}
}

}
