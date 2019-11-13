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
#include "Parameters.h"
#include "Netlist.h"
#include "RoutingLayer.h"
#include "RoutingTracks.h"
#include "../include/FastRoute.h"

class FastRouteKernel {
protected:
        Netlist _netlist;
        Grid _grid;
        std::vector<RoutingLayer> _routingLayers;
        std::vector<RoutingTracks> _allRoutingTracks;

private:
	DBWrapper _dbWrapper;
        Parameters* _parms;
        FastRoute::FT _fastRoute;
        std::string _outfile = "output.guide";
        float _adjustment = 0.0;
        int _minRoutingLayer = 1;
        int _maxRoutingLayer = -1;
        bool _unidirectionalRoute = false;
        int _fixLayer;
        bool _interactiveMode;

        std::vector<int> _vCapacities;
        std::vector<int> _hCapacities;
        std::vector<FastRoute::NET> _result;
        std::map<std::string, int> _netsDegree;

        void initGrid();
        void initRoutingLayers();
        void initRoutingTracks();
        void setCapacities();
        void setSpacingsAndMinWidths();
        void initializeNets();
        void computeGridAdjustments();
        void computeTrackAdjustments();
        void computeUserAdjustments();
        void computeObstaclesAdjustments();
        
        RoutingLayer getRoutingLayerByIndex(int index);
        RoutingTracks getRoutingTracksByIndex(int layer);
        void addRemainingGuides(std::vector<FastRoute::NET> &globalRoute);
        void mergeBox(std::vector<Box>& guideBox);
        Box globalRoutingToBox(const FastRoute::ROUTE &route);

public:
	FastRouteKernel(Parameters& parms);
	FastRouteKernel() = default;
        
	void parseLef(const std::string& file) { _dbWrapper.parseLEF(file); }
        void parseDef(const std::string& file) { _dbWrapper.parseDEF(file); }
        
        void setAdjustment(const float adjustment) { _adjustment = adjustment; }
        void setMinRoutingLayer(const int minLayer) { _minRoutingLayer = minLayer; }
        void setMaxRoutingLayer(const int maxLayer) { _maxRoutingLayer = maxLayer; }
        void setUnidirectionalRoute(const bool unidirRoute) { _unidirectionalRoute = unidirRoute; }
        void setOutputFile(const std::string& outfile) { _outfile = outfile; }
        
        void printGrid();
        
        void writeGuides();
        void startFastRoute();
        void runFastRoute();
        int run();
        
        void resetFastRoute();
};

#endif /* __FASTROUTEKERNEL_H_ */
