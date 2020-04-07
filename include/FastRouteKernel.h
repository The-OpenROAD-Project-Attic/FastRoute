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
#include "RoutingLayer.h"
#include "RoutingTracks.h"

namespace FastRoute {

class FastRouteKernel {
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
        
        void setGridOrigin(long x, long y) {
                _gridOrigin = Coordinate(x, y);
        }
        
        void setPDRevForHighFanout(int pdRevForHighFanout) {
                _pdRevForHighFanout = pdRevForHighFanout;
        }

        void setAllowOverflow(bool allowOverflow) {
                _allowOverflow = allowOverflow;
        }

        void setRouteNetsWithPad(bool routeNetsWithPad) {
                _routeNetsWithPad = routeNetsWithPad;
        }

        void printGrid();
        void printHeader();
        
        // flow functions
        void writeGuides();
        void startFastRoute();
        void runFastRoute();
        int run();
        
        // temporary for congestion drive replace
        void writeRoute();
        void writeEst();
};

}

#endif /* __FASTROUTEKERNEL_H_ */
