////////////////////////////////////////////////////////////////////////////////
//// Authors: Vitor Bandeira, Mateus Fogaça, Eder Matheus Monteiro e Isadora
//// Oliveira
////          (Advisor: Ricardo Reis)
////
//// BSD 3-Clause License
////
//// Copyright (c) 2019, Federal University of Rio Grande do Sul (UFRGS)
//// All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification, are permitted provided that the following conditions are met:
////
//// * Redistributions of source code must retain the above copyright notice, this
////   list of conditions and the following disclaimer.
////
//// * Redistributions in binary form must reproduce the above copyright notice,
////   this list of conditions and the following disclaimer in the documentation
////   and/or other materials provided with the distribution.
////
//// * Neither the name of the copyright holder nor the names of its
////   contributors may be used to endorse or promote products derived from
////   this software without specific prior written permission.
////
//// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//// POSSIBILITY OF SUCH DAMAGE.
//////////////////////////////////////////////////////////////////////////////////

#ifndef __FASTROUTEKERNEL_H_
#define __FASTROUTEKERNEL_H_

#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>
#include <string>
#include <utility>
#include <fstream>
#include <istream>

#include "Coordinate.h"
#include "Box.h"
#include "DBWrapper.h"
#include "Grid.h"
#include "Netlist.h"
#include "RoutingLayer.h"
#include "RoutingTracks.h"
#include "../include/FastRoute.h"

namespace FastRoute {

class FastRouteKernel {
protected:
        Netlist _netlist;
        Grid _grid;
        std::vector<RoutingLayer> _routingLayers;
        std::vector<RoutingTracks> _allRoutingTracks;

private:
        DBWrapper _dbWrapper;
        FT _fastRoute;
        std::string _outfile = "output.guide";
        float _adjustment = 0.0;
        int _minRoutingLayer = 1;
        int _maxRoutingLayer = -1;
        bool _unidirectionalRoute = false;
        int _fixLayer;
        bool _interactiveMode;
        bool _clockNetRouting;
        unsigned _dbId;
        const int _selectedMetal = 3;
        int _overflowIterations = 50;
        
        // Layer adjustment variables
        std::vector<int> _layersToAdjust;
        std::vector<float> _layersReductionPercentage;
        
        // Region adjustment variables
        std::vector<int> regionsMinX;
        std::vector<int> regionsMinY;
        std::vector<int> regionsMaxX;
        std::vector<int> regionsMaxY;
        std::vector<int> regionsLayer;
        std::vector<float> regionsReductionPercentage;

        std::vector<int> _vCapacities;
        std::vector<int> _hCapacities;
        std::vector<FastRoute::NET> _result;
        std::map<std::string, int> _netsDegree;

        // Clock net routing variables
        bool _pdRev;
        float _alpha = 0.4;
        int _verbose = 0;
        std::map<std::string, float> _netsAlpha;
        
        // main functions
        void initGrid();
        void initRoutingLayers();
        void initRoutingTracks();
        void setCapacities();
        void setSpacingsAndMinWidths();
        void initializeNets();
        void computeGridAdjustments();
        void computeTrackAdjustments();
        void computeUserGlobalAdjustments();
        void computeUserLayerAdjustments();
        void computeRegionAdjustments(Coordinate lowerBound, Coordinate upperBound, int layer, float reductionPercentage);
        void computeObstaclesAdjustments();
        
        // aux functions
        RoutingLayer getRoutingLayerByIndex(int index);
        RoutingTracks getRoutingTracksByIndex(int layer);
        void addRemainingGuides(std::vector<FastRoute::NET> &globalRoute);
        void mergeBox(std::vector<Box>& guideBox);
        Box globalRoutingToBox(const FastRoute::ROUTE &route);
        
        // check functions
        void checkPinPlacement();

public:
	FastRouteKernel();
        
        void setAdjustment(const float adjustment) { _adjustment = adjustment; }
        void setMinRoutingLayer(const int minLayer) { _minRoutingLayer = minLayer; }
        void setMaxRoutingLayer(const int maxLayer) { _maxRoutingLayer = maxLayer; }
        void setUnidirectionalRoute(const bool unidirRoute) { _unidirectionalRoute = unidirRoute; }
        void setClockNetRouting(const bool clockNetRouting) { _clockNetRouting = clockNetRouting; }
        void setPDRev(const bool pdRev) { _pdRev = pdRev; }
        void setAlpha(const float alpha) { _alpha = alpha; }
        void setOutputFile(const std::string& outfile) { _outfile = outfile; }
        void setPitchesInTile(const int pitchesInTile) { _grid.setPitchesInTile(pitchesInTile); }
        void setDbId(unsigned idx) { _dbId = idx; }
        unsigned getDbId() { return _dbId; }
        
        void addLayerAdjustment(int layer, float reductionPercentage) {
                _layersToAdjust.push_back(layer);
                _layersReductionPercentage.push_back(reductionPercentage);
        }
        
        void addRegionAdjustment(int minX, int minY, int maxX, int maxY,
                                 int layer, float reductionPercentage) {
                regionsMinX.push_back(minX);
                regionsMinY.push_back(minY);
                regionsMaxX.push_back(maxX);
                regionsMaxY.push_back(maxY);
                regionsLayer.push_back(layer);
                regionsReductionPercentage.push_back(reductionPercentage);
        }
        
        void addAlphaForNet(char * netName, float alpha) {
                std::string name(netName);
                _netsAlpha[name] = alpha;
        }
        
        void setVerbose(const int v) {
                _verbose = v;
        }
        
        void setOverflowIterations(int iterations) {
                _overflowIterations = iterations;
        }
        
        void printGrid();
        void printHeader();
        
        // flow functions
        void writeGuides();
        void startFastRoute();
        void runFastRoute();
        int run();
};

}

#endif /* __FASTROUTEKERNEL_H_ */
