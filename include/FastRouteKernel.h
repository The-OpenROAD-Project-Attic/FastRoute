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
#include <map>

namespace FastRoute {

class FT;
class Box;
class Coordinate;
class DBWrapper;
class Grid;
class Pin;
class Netlist;
class RoutingTracks;
class RoutingLayer;
struct NET;
struct ROUTE;
struct PIN;

class FastRouteKernel {
protected:
        Netlist* _netlist = nullptr;
        Grid* _grid = nullptr;
        std::vector<RoutingLayer> *_routingLayers = nullptr;
        std::vector<RoutingTracks> *_allRoutingTracks = nullptr;

private:
        // Objects variables
        DBWrapper* _dbWrapper = nullptr;
        FT* _fastRoute = nullptr;
        Coordinate* _gridOrigin = nullptr;
        std::vector<FastRoute::NET> *_result;

        // Flow variables
        std::string _outfile;
        std::string _congestFile;
        float _adjustment;
        int _minRoutingLayer;
        int _maxRoutingLayer;
        bool _unidirectionalRoute;
        int _fixLayer;
        bool _clockNetRouting;
        unsigned _dbId;
        const int _selectedMetal = 3;
        const float transitionLayerAdjust = 0.6;
        int _overflowIterations;
        int _pdRevForHighFanout;
        bool _allowOverflow;
        bool _reportCongest;
        std::vector<int> _vCapacities;
        std::vector<int> _hCapacities;
        std::map<std::string, int> _netsDegree;
        unsigned _seed;

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

        // Clock net routing variables
        bool _pdRev;
        float _alpha;
        int _verbose;
        std::map<std::string, float> _netsAlpha;
        
        // temporary for congestion driven replace
        int _numAdjusts = 0;
        
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
        void computeRegionAdjustments(const Coordinate& lowerBound, const Coordinate& upperBound, int layer, float reductionPercentage);
        void computeObstaclesAdjustments();
        void computeWirelength();
        
        // aux functions
        RoutingLayer getRoutingLayerByIndex(int index);
        RoutingTracks getRoutingTracksByIndex(int layer);
        void addRemainingGuides(std::vector<FastRoute::NET> &globalRoute);
        void mergeBox(std::vector<Box>& guideBox);
        Box globalRoutingToBox(const FastRoute::ROUTE &route);
        bool segmentsOverlaps(const ROUTE& seg0, const ROUTE& seg1, ROUTE &newSeg);
        void mergeSegments(FastRoute::NET &net);
        bool pinOverlapsWithSingleTrack(const Pin& pin, Coordinate &trackPosition);
        
        // check functions
        void checkPinPlacement();

public:
        struct EST_ {
                std::string netName;
                int netId;
                int numSegments;
                std::vector<long> initX;
                std::vector<long> initY;
                std::vector<int> initLayer;
                std::vector<long> finalX;
                std::vector<long> finalY;
                std::vector<int> finalLayer;
        };

        struct ADJUSTMENT_ {
                int firstX;
                int firstY;
                int firstLayer;
                int finalX;
                int finalY;
                int finalLayer;
                int edgeCapacity;
        };

        struct ROUTE_ {
                int gridCountX;
                int gridCountY;
                int numLayers;
                std::vector<int> verticalEdgesCapacities;
                std::vector<int> horizontalEdgesCapacities;
                std::vector<int> minWireWidths;
                std::vector<int> minWireSpacings;
                std::vector<int> viaSpacings;
                long gridOriginX;
                long gridOriginY;
                long tileWidth;
                long tileHeight;
                int blockPorosity;
                int numAdjustments;
                std::vector<ADJUSTMENT_> adjustments;
        };

        FastRouteKernel();
        ~FastRouteKernel();
        void init();
        void reset();
        void resetResources();
        
        void setAdjustment(const float adjustment);
        void setMinRoutingLayer(const int minLayer);
        void setMaxRoutingLayer(const int maxLayer);
        void setUnidirectionalRoute(const bool unidirRoute);
        void setClockNetRouting(const bool clockNetRouting);
        void setPDRev(const bool pdRev);
        void setAlpha(const float alpha);
        void setOutputFile(const std::string& outfile);
        void setPitchesInTile(const int pitchesInTile);
        void setDbId(unsigned idx);
        void setSeed(unsigned seed);
        unsigned getDbId();
        void addLayerAdjustment(int layer, float reductionPercentage);
        void addRegionAdjustment(int minX, int minY, int maxX, int maxY, int layer, float reductionPercentage);
        void addAlphaForNet(char * netName, float alpha);
        void setVerbose(const int v);
        void setOverflowIterations(int iterations);
        void setGridOrigin(long x, long y);
        void setPDRevForHighFanout(int pdRevForHighFanout);
        void setAllowOverflow(bool allowOverflow);
        void setReportCongestion(char * congestFile);
        void printGrid();
        void printHeader();
        
        // flow functions
        void writeGuides();
        void startFastRoute();
        void runFastRoute();
        
        // congestion drive replace functions
        ROUTE_ getRoute();
        std::vector<EST_> getEst();
};

}

#endif /* __FASTROUTEKERNEL_H_ */
