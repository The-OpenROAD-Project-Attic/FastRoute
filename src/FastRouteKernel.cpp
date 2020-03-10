////////////////////////////////////////////////////////////////////////////////
// Authors: Vitor Bandeira, Mateus Fogaça, Eder Matheus Monteiro e Isadora
// Oliveira
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

#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>
#include <string>
#include <utility>
#include <fstream>
#include <istream>

#include "FastRouteKernel.h"

namespace FastRoute {

FastRouteKernel::FastRouteKernel() {
        _dbWrapper = DBWrapper(_netlist, _grid);
        _interactiveMode = true;
}

int FastRouteKernel::run() {
        printHeader();
        if (_unidirectionalRoute) {
                _minRoutingLayer = 2;
                _fixLayer = 1;
        }

        if (_maxRoutingLayer == -1) {
                std::cout << " > Computing max routing layer...\n";
                _maxRoutingLayer = _dbWrapper.computeMaxRoutingLayer();
                std::cout << " > Computing max routing layer... Done!\n";
        }

        if (_clockNetRouting && _pdRev){
                _fastRoute.usePdRev();
                _fastRoute.setAlpha(_alpha);
        }
        
        if (_maxRoutingLayer < _selectedMetal) {
                _dbWrapper.setSelectedMetal(_maxRoutingLayer);
        }
        
        _fastRoute.setVerbose(_verbose);
        _fastRoute.setOverflowIterations(_overflowIterations);
        
        std::cout << " > Initializing grid...\n";
        initGrid();
        std::cout << " > Initializing grid... Done!\n";
        
        std::cout << " > Initializing routing layers...\n";
        initRoutingLayers();
        std::cout << " > Initializing routing layers... Done!\n";
        
        std::cout << " > Initializing routing tracks...\n";
        initRoutingTracks();
        std::cout << " > Initializing routing tracks... Done!\n";
            
        std::cout << " > Setting capacities...\n";
        setCapacities();
        std::cout << " > Setting capacities... Done!\n";
        
        std::cout << " > Setting spacings and widths...\n";
        setSpacingsAndMinWidths();
        std::cout << " > Setting spacings and widths... Done!\n";
        
        std::cout << " > Initializing nets...\n";
        initializeNets();
        std::cout << " > Initializing nets... Done!\n";
        
        std::cout << " > Adjusting grid...\n";
        computeGridAdjustments();
        std::cout << " > Adjusting grid... Done!\n";
        
        std::cout << " > Computing track adjustments...\n";
        computeTrackAdjustments();
        std::cout << " > Computing track adjustments... Done!\n";

        std::cout << " > Computing obstacles adjustments...\n";
        computeObstaclesAdjustments();
        std::cout << " > Computing obstacles adjustments... Done!\n";
                
        std::cout << " > Computing user defined global adjustments...\n";
        computeUserGlobalAdjustments();
        std::cout << " > Computing user defined global adjustments... Done!\n";
        
        std::cout << " > Computing user defined layers adjustments...\n";
        computeUserLayerAdjustments();
        std::cout << " > Computing user defined layers adjustments... Done!\n";
        
        for (int i = 0; i < regionsReductionPercentage.size(); i++) {
                if (regionsLayer[i] < 1)
                        break;
                
                std::cout << " > Adjusting specific region in layer " << regionsLayer[i] << "...\n";
                Coordinate lowerLeft = Coordinate(regionsMinX[i], regionsMinY[i]);
                Coordinate upperRight = Coordinate(regionsMaxX[i], regionsMaxY[i]);
                computeRegionAdjustments(lowerLeft, upperRight, regionsLayer[i], regionsReductionPercentage[i]);
        }

        _fastRoute.initAuxVar();
        
        std::cout << " > Running FastRoute...\n";
        _fastRoute.run(_result);
        std::cout << " > Running FastRoute... Done!\n";
        
        writeGuides();
        
        return 0;
}

void FastRouteKernel::startFastRoute() {
        printHeader();
        _dbWrapper.setDB(_dbId);
        if (_unidirectionalRoute) {
                _minRoutingLayer = 2;
                _fixLayer = 1;
        }

        if (_maxRoutingLayer == -1) {
                std::cout << " > Computing max routing layer...\n";
                _maxRoutingLayer = _dbWrapper.computeMaxRoutingLayer();
                std::cout << " > Computing max routing layer... Done!\n";
        }
        
        if (_maxRoutingLayer < _selectedMetal) {
                _dbWrapper.setSelectedMetal(_maxRoutingLayer);
        }

        if (_clockNetRouting && _pdRev) {
                _fastRoute.usePdRev();
                _fastRoute.setAlpha(_alpha);
        }
        
        _fastRoute.setVerbose(_verbose);
        _fastRoute.setOverflowIterations(_overflowIterations);
        
        std::cout << " > Params:\n";
        std::cout << " > ---- Min routing layer: " << _minRoutingLayer << "\n";
        std::cout << " > ---- Max routing layer: " << _maxRoutingLayer << "\n";
        std::cout << " > ---- Global adjustment: " << _adjustment << "\n";
        std::cout << " > ---- Unidirectional routing: " << _unidirectionalRoute << "\n";
        std::cout << " > ---- Clock net routing: " << _clockNetRouting << "\n";
        
        std::cout << " > Initializing grid...\n";
        initGrid();
        std::cout << " > Initializing grid... Done!\n";
        
        std::cout << " > Initializing routing layers...\n";
        initRoutingLayers();
        std::cout << " > Initializing routing layers... Done!\n";
        
        std::cout << " > Initializing routing tracks...\n";
        initRoutingTracks();
        std::cout << " > Initializing routing tracks... Done!\n";
            
        std::cout << " > Setting capacities...\n";
        setCapacities();
        std::cout << " > Setting capacities... Done!\n";
        
        std::cout << " > Setting spacings and widths...\n";
        setSpacingsAndMinWidths();
        std::cout << " > Setting spacings and widths... Done!\n";
        
        std::cout << " > Initializing nets...\n";
        initializeNets();
        std::cout << " > Initializing nets... Done!\n";
        
        std::cout << " > Adjusting grid...\n";
        computeGridAdjustments();
        std::cout << " > Adjusting grid... Done!\n";
        
        std::cout << " > Computing track adjustments...\n";
        computeTrackAdjustments();
        std::cout << " > Computing track adjustments... Done!\n";

        std::cout << " > Computing obstacles adjustments...\n";
        computeObstaclesAdjustments();
        std::cout << " > Computing obstacles adjustments... Done!\n";
                
        std::cout << " > Computing user defined adjustments...\n";
        computeUserGlobalAdjustments();
        std::cout << " > Computing user defined adjustments... Done!\n";
        
        std::cout << " > Computing user defined layers adjustments...\n";
        computeUserLayerAdjustments();
        std::cout << " > Computing user defined layers adjustments... Done!\n";
        
        for (int i = 0; i < regionsReductionPercentage.size(); i++) {
                if (regionsLayer[i] < 1)
                        break;
                
                std::cout << " > Adjusting specific region in layer " << regionsLayer[i] << "...\n";
                Coordinate lowerLeft = Coordinate(regionsMinX[i], regionsMinY[i]);
                Coordinate upperRight = Coordinate(regionsMaxX[i], regionsMaxY[i]);
                computeRegionAdjustments(lowerLeft, upperRight, regionsLayer[i], regionsReductionPercentage[i]);
        }

        _fastRoute.initAuxVar();
}

void FastRouteKernel::runFastRoute() {
        std::cout << " > Running FastRoute...\n";
        _fastRoute.run(_result);
        std::cout << " > Running FastRoute... Done!\n";
}

void FastRouteKernel::initGrid() {        
        _dbWrapper.initGrid(_maxRoutingLayer);
        _dbWrapper.computeCapacities(_maxRoutingLayer);
        _dbWrapper.computeSpacingsAndMinWidth(_maxRoutingLayer);
        _dbWrapper.initObstacles();
        
        _fastRoute.setLowerLeft(_grid.getLowerLeftX(), _grid.getLowerLeftY());
        _fastRoute.setTileSize(_grid.getTileWidth(), _grid.getTileHeight());
        _fastRoute.setGridsAndLayers(_grid.getXGrids(), _grid.getYGrids(), _grid.getNumLayers());
}

void FastRouteKernel::initRoutingLayers() {
        _dbWrapper.initRoutingLayers(_routingLayers, _maxRoutingLayer);
        
        RoutingLayer routingLayer = getRoutingLayerByIndex(1);
        _fastRoute.setLayerOrientation(routingLayer.getPreferredDirection());
}

void FastRouteKernel::initRoutingTracks() {
        _dbWrapper.initRoutingTracks(_allRoutingTracks, _maxRoutingLayer);
}

void FastRouteKernel::setCapacities() {
        for (int l = 1; l <= _grid.getNumLayers(); l++) {
                if (l < _minRoutingLayer || l > _maxRoutingLayer) {
                        _fastRoute.addHCapacity(0, l);
                        _fastRoute.addVCapacity(0, l);

                        _hCapacities.push_back(0);
                        _vCapacities.push_back(0);
                } else {
                        _fastRoute.addHCapacity(_grid.getHorizontalEdgesCapacities()[l-1], l);
                        _fastRoute.addVCapacity(_grid.getVerticalEdgesCapacities()[l-1], l);

                        _hCapacities.push_back(_grid.getHorizontalEdgesCapacities()[l-1]);
                        _vCapacities.push_back(_grid.getVerticalEdgesCapacities()[l-1]);
                }
        }
}

void FastRouteKernel::setSpacingsAndMinWidths() {
        for (int l = 1; l <= _grid.getNumLayers(); l++) {
                _fastRoute.addMinSpacing(_grid.getSpacings()[l-1], l);
                _fastRoute.addMinWidth(_grid.getMinWidths()[l-1], l);
                _fastRoute.addViaSpacing(1, l);
        }
}

void FastRouteKernel::initializeNets() {
        _dbWrapper.initNetlist();
        
        std::cout << " > ----Checking pin placement...\n";
        checkPinPlacement();
        std::cout << " > ----Checking pin placement... Done!\n";
        
        int idx = 0;
        _fastRoute.setNumberNets(_netlist.getNetCount());
        _fastRoute.setMaxNetDegree(_netlist.getMaxNetDegree());
        
        for (Net net : _netlist.getNets()) {
                float netAlpha = _alpha;

                if (net.getNumPins() == 1) {
                        continue;
                }
                
                if (_clockNetRouting && net.getSignalType() != "CLOCK") {
                        continue;
                }
                
                _netsDegree[net.getName()] = net.getNumPins();
                
                std::vector<FastRoute::PIN> pins;
                for (Pin pin : net.getPins()) {
                        Coordinate pinPosition;
                        int topLayer = pin.getTopLayer();
                        
                        std::vector<Box> pinBoxes = pin.getBoxes()[topLayer];
                        std::vector<Coordinate> pinPositionsOnGrid;
                        Coordinate posOnGrid;
                                
                        for (Box pinBox : pinBoxes) {
                                posOnGrid = _grid.getPositionOnGrid(pinBox.getMiddle());
                                pinPositionsOnGrid.push_back(posOnGrid);
                        }
                        
                        int votes = -1;
                        
                        for (Coordinate pos : pinPositionsOnGrid) {
                                int equals = std::count(pinPositionsOnGrid.begin(),
                                                        pinPositionsOnGrid.end(),
                                                        pos);
                                if (equals > votes) {
                                        pinPosition = pos;
                                        votes = equals;
                                }
                        }
                        
                        FastRoute::PIN grPin;
                        grPin.x = pinPosition.getX();
                        grPin.y = pinPosition.getY();
                        grPin.layer = topLayer;
                        pins.push_back(grPin);
                }
                
                FastRoute::PIN grPins[pins.size()];
                char netName[net.getName().size() + 1];
                strcpy(netName, net.getName().c_str());
                int count = 0;
                
                for (FastRoute::PIN pin : pins) {
                        grPins[count] = pin;
                        count ++;
                }
                
                if (_netsAlpha.find(net.getName()) != _netsAlpha.end()) {
                        netAlpha = _netsAlpha[net.getName()];
                }
                
                _fastRoute.addNet(netName, idx, pins.size(), 1, grPins, netAlpha);
                idx++;
        }

        _fastRoute.initEdges();
}

void FastRouteKernel::computeGridAdjustments() {
        Coordinate upperDieBounds = Coordinate(_grid.getUpperRightX(),
                                               _grid.getUpperRightY());
        DBU hSpace;
        DBU vSpace;

        int xGrids = _grid.getXGrids();
        int yGrids = _grid.getYGrids();

        Coordinate upperGridBounds = Coordinate(xGrids*_grid.getTileWidth(), yGrids*_grid.getTileHeight());
        DBU xExtra = upperDieBounds.getX() - upperGridBounds.getX();
        DBU yExtra = upperDieBounds.getY() - upperGridBounds.getY();

        for (int layer = 1; layer <= _grid.getNumLayers(); layer++) {
                hSpace = 0;
                vSpace = 0;
                RoutingLayer routingLayer = getRoutingLayerByIndex(layer);
                
                if (layer < _minRoutingLayer || (layer > _maxRoutingLayer &&
                    _maxRoutingLayer > 0))
                        continue;

                int newVCapacity = 0;
                int newHCapacity = 0;
                
                if (routingLayer.getPreferredDirection() == RoutingLayer::HORIZONTAL) {
                        hSpace = _grid.getMinWidths()[layer-1];
                        newHCapacity = std::floor((_grid.getTileHeight() + yExtra)/hSpace);
                } else if (routingLayer.getPreferredDirection() == RoutingLayer::VERTICAL) {
                        vSpace = _grid.getMinWidths()[layer-1];
                        newVCapacity = std::floor((_grid.getTileWidth() + xExtra)/vSpace);
                } else {
                    std::cout << " > [ERROR] Layer spacing not found. Exiting...\n";
                    std::exit(1);
                }
                
                int numAdjustments = 0;
                for (int i = 1; i < yGrids; i++)
                        numAdjustments++;
                for (int i = 1; i < xGrids; i++)
                        numAdjustments++;
                _fastRoute.setNumAdjustments(numAdjustments);

                if (!_grid.isPerfectRegularX()) {
                        for (int i = 1; i < yGrids; i++) {
                                _fastRoute.addAdjustment(xGrids - 1, i - 1, layer, xGrids - 1, i, layer, newVCapacity, false);
                        }
                }
                if (!_grid.isPerfectRegularY()) {
                        for (int i = 1; i < xGrids; i++) {
                                _fastRoute.addAdjustment(i - 1, yGrids - 1, layer, i, yGrids - 1, layer, newHCapacity, false);
                        }
                }
        }
}

void FastRouteKernel::computeTrackAdjustments() {
        Coordinate upperDieBounds = Coordinate(_grid.getUpperRightX(),
                                               _grid.getUpperRightY());
        for (RoutingLayer layer : _routingLayers) {
                DBU trackLocation;
                int numInitAdjustments = 0;
                int numFinalAdjustments = 0;
                DBU trackSpace;
                int numTracks = 0;
                
                if (layer.getIndex() < _minRoutingLayer ||
                    (layer.getIndex() > _maxRoutingLayer && _maxRoutingLayer > 0))
                        continue;
                
                if (layer.getPreferredDirection() == RoutingLayer::HORIZONTAL) {
                        RoutingTracks routingTracks = getRoutingTracksByIndex(layer.getIndex());
                        trackLocation = routingTracks.getLocation();
                        trackSpace = routingTracks.getSpace();
                        numTracks = routingTracks.getNumTracks();
                        
                        if (numTracks > 0) {
                                DBU finalTrackLocation = trackLocation + (trackSpace * (numTracks-1));
                                DBU remainingFinalSpace = upperDieBounds.getY() - finalTrackLocation;
                                DBU extraSpace = upperDieBounds.getY() - (_grid.getTileHeight() * _grid.getYGrids());
                                if (_grid.isPerfectRegularY()) {
                                        numFinalAdjustments = std::ceil((float)remainingFinalSpace/_grid.getTileHeight());
                                } else {
                                        if (remainingFinalSpace != 0){
                                                DBU finalSpace = remainingFinalSpace - extraSpace;
                                                if (finalSpace <= 0)
                                                        numFinalAdjustments = 1;
                                                else
                                                        numFinalAdjustments = std::ceil((float)finalSpace/_grid.getTileHeight());
                                        }
                                        else
                                                numFinalAdjustments = 0;
                                }
                                
                                numFinalAdjustments *= _grid.getXGrids();
                                numInitAdjustments = std::ceil((float)trackLocation/_grid.getTileHeight());
                                numInitAdjustments *= _grid.getXGrids();
                                _fastRoute.setNumAdjustments(numInitAdjustments + numFinalAdjustments);
                                
                                int y = 0;
                                while (trackLocation >= _grid.getTileHeight()){
                                        for (int x = 1; x < _grid.getXGrids(); x++){
                                                _fastRoute.addAdjustment(x - 1, y, layer.getIndex(), x, y, layer.getIndex(), 0);
                                        }
                                        y++;
                                        trackLocation -= _grid.getTileHeight();
                                }
                                if (trackLocation > 0){
                                        DBU remainingTile = _grid.getTileHeight() - trackLocation;
                                        int newCapacity = std::floor((float)remainingTile/trackSpace);
                                        for (int x = 1; x < _grid.getXGrids(); x++){
                                                _fastRoute.addAdjustment(x - 1, y, layer.getIndex(), x, y, layer.getIndex(), newCapacity);
                                        }
                                }
                                
                                y = _grid.getYGrids() - 1;
                                while (remainingFinalSpace >= _grid.getTileHeight() + extraSpace){
                                        for (int x = 1; x < _grid.getXGrids(); x++){
                                                _fastRoute.addAdjustment(x - 1, y, layer.getIndex(), x, y, layer.getIndex(), 0);
                                        }
                                        y--;
                                        remainingFinalSpace -= (_grid.getTileHeight() + extraSpace);
                                        extraSpace = 0;
                                }
                                if (remainingFinalSpace > 0){
                                        DBU remainingTile = (_grid.getTileHeight() + extraSpace) - remainingFinalSpace;
                                        int newCapacity = std::floor((float)remainingTile/trackSpace);
                                        for (int x = 1; x < _grid.getXGrids(); x++){
                                                _fastRoute.addAdjustment(x - 1, y, layer.getIndex(), x, y, layer.getIndex(), newCapacity);
                                        }
                                }
                        }
                } else {
                        RoutingTracks routingTracks = getRoutingTracksByIndex(layer.getIndex());
                        trackLocation = routingTracks.getLocation();
                        trackSpace = routingTracks.getSpace();
                        numTracks = routingTracks.getNumTracks();
                        
                        if (numTracks > 0) {
                                DBU finalTrackLocation = trackLocation + (trackSpace * (numTracks-1));
                                DBU remainingFinalSpace = upperDieBounds.getX() - finalTrackLocation;
                                DBU extraSpace = upperDieBounds.getX() - (_grid.getTileWidth() * _grid.getXGrids());
                                if (_grid.isPerfectRegularX()) {
                                        numFinalAdjustments = std::ceil((float)remainingFinalSpace/_grid.getTileWidth());
                                } else {
                                        if (remainingFinalSpace != 0){
                                                DBU finalSpace = remainingFinalSpace - extraSpace;
                                                if (finalSpace <= 0)
                                                        numFinalAdjustments = 1;
                                                else
                                                        numFinalAdjustments = std::ceil((float)finalSpace/_grid.getTileWidth());
                                        }
                                        else
                                                numFinalAdjustments = 0;
                                }
                                
                                numFinalAdjustments *= _grid.getYGrids();
                                numInitAdjustments = std::ceil((float)trackLocation/_grid.getTileWidth());
                                numInitAdjustments *= _grid.getYGrids();
                                _fastRoute.setNumAdjustments(numInitAdjustments + numFinalAdjustments);
                                
                                int x = 0;
                                while (trackLocation >= _grid.getTileWidth()){
                                        for (int y = 1; y < _grid.getYGrids(); y++){
                                                _fastRoute.addAdjustment(x, y - 1, layer.getIndex(), x, y, layer.getIndex(), 0);
                                        }
                                        x++;
                                        trackLocation -= _grid.getTileWidth();
                                }
                                if (trackLocation > 0){
                                        DBU remainingTile = _grid.getTileWidth() - trackLocation;
                                        int newCapacity = std::floor((float)remainingTile/trackSpace);
                                        for (int y = 1; y < _grid.getYGrids(); y++) {
                                                _fastRoute.addAdjustment(x, y - 1, layer.getIndex(), x, y, layer.getIndex(), newCapacity);
                                        }
                                }
                                
                                x = _grid.getXGrids() - 1;
                                while (remainingFinalSpace >= _grid.getTileWidth() + extraSpace){
                                        for (int y = 1; y < _grid.getYGrids(); y++){
                                                _fastRoute.addAdjustment(x, y - 1, layer.getIndex(), x, y, layer.getIndex(), 0);
                                        }
                                        x--;
                                        remainingFinalSpace -= (_grid.getTileWidth() + extraSpace);
                                        extraSpace = 0;
                                }
                                if (remainingFinalSpace > 0){
                                        DBU remainingTile = (_grid.getTileWidth() + extraSpace) - remainingFinalSpace;
                                        int newCapacity = std::floor((float)remainingTile/trackSpace);
                                        for (int y = 1; y < _grid.getYGrids(); y++){
                                                _fastRoute.addAdjustment(x, y - 1, layer.getIndex(), x, y, layer.getIndex(), newCapacity);
                                        }
                                }
                        }
                }
        }
}

void FastRouteKernel::computeUserGlobalAdjustments() {
        if (_adjustment == 0.0)
                return;

        int xGrids = _grid.getXGrids();
        int yGrids = _grid.getYGrids();
        
        int numAdjustments = 0;

        for (int layer = 1; layer <= _grid.getNumLayers(); layer++) {
                for (int y = 0; y < yGrids; y++) {
                        for (int x = 0; x < xGrids; x++) {
                                numAdjustments++;
                        }
                }
        }

        numAdjustments *= 2;
        _fastRoute.setNumAdjustments(numAdjustments);

        for (int layer = 1; layer <= _grid.getNumLayers(); layer++) {
                if (_hCapacities[layer - 1] != 0) {
                        for (int y = 1; y < yGrids; y++) {
                                for (int x = 1; x < xGrids; x++) {
                                        int edgeCap = _fastRoute.getEdgeCapacity(x - 1, y - 1, layer, x, y - 1, layer);
                                        int newHCapacity = std::floor((float)edgeCap * (1 - _adjustment));
                                        _fastRoute.addAdjustment(x - 1, y - 1, layer, x, y - 1, layer, newHCapacity);
                                }
                        }
                }

                if (_vCapacities[layer - 1] != 0) {
                        for (int x = 1; x < xGrids; x++) {
                                for (int y = 1; y < yGrids; y++) {
                                        int edgeCap = _fastRoute.getEdgeCapacity(x - 1, y - 1, layer, x - 1, y, layer);
                                        int newVCapacity = std::floor((float)edgeCap * (1 - _adjustment));
                                        _fastRoute.addAdjustment(x - 1, y - 1, layer, x - 1, y, layer, newVCapacity);
                                }
                        }
                }
        }
}

void FastRouteKernel::computeUserLayerAdjustments() {
        int xGrids = _grid.getXGrids();
        int yGrids = _grid.getYGrids();
        
        int numAdjustments = 0;

        for (int layer = 0; layer < _layersToAdjust.size(); layer++) {
                for (int y = 0; y < yGrids; y++) {
                        for (int x = 0; x < xGrids; x++) {
                                numAdjustments++;
                        }
                }
        }

        numAdjustments *= 2;
        _fastRoute.setNumAdjustments(numAdjustments);

        for (int idx = 0; idx < _layersToAdjust.size(); idx++) {
                int layer = _layersToAdjust[idx];
                float adjustment = _layersReductionPercentage[idx];
                if (_hCapacities[layer - 1] != 0) {
                        for (int y = 1; y < yGrids; y++) {
                                for (int x = 1; x < xGrids; x++) {
                                        int edgeCap = _fastRoute.getEdgeCapacity(x - 1, y - 1, layer, x, y - 1, layer);
                                        int newHCapacity = std::floor((float)edgeCap * (1 - adjustment));
                                        _fastRoute.addAdjustment(x - 1, y - 1, layer, x, y - 1, layer, newHCapacity);
                                }
                        }
                }

                if (_vCapacities[layer - 1] != 0) {
                        for (int x = 1; x < xGrids; x++) {
                                for (int y = 1; y < yGrids; y++) {
                                        int edgeCap = _fastRoute.getEdgeCapacity(x - 1, y - 1, layer, x - 1, y, layer);
                                        int newVCapacity = std::floor((float)edgeCap * (1 - adjustment));
                                        _fastRoute.addAdjustment(x - 1, y - 1, layer, x - 1, y, layer, newVCapacity);
                                }
                        }
                }
        }
}

void FastRouteKernel::computeRegionAdjustments(Coordinate lowerBound, Coordinate upperBound,
                                               int layer, float reductionPercentage) {
        Box firstTileBox;
        Box lastTileBox;
        std::pair<Grid::TILE, Grid::TILE> tilesToAdjust;
        
        Box dieBox = Box(_grid.getLowerLeftX(), _grid.getLowerLeftY(),
                         _grid.getUpperRightX(), _grid.getUpperRightY(), -1);
        
        if ((dieBox.getLowerBound().getX() > lowerBound.getX() && dieBox.getLowerBound().getY() > lowerBound.getY()) ||
            (dieBox.getUpperBound().getX() < upperBound.getX() && dieBox.getUpperBound().getY() < upperBound.getY())) {
                std::cout << " > [ERROR] Informed region is outside die area!\n";
                std::cout << "Informed region: (" << lowerBound.getX() << ", " << lowerBound.getY() << "); ("
                          << upperBound.getX() << ", " << upperBound.getY() << ")\n";
                std::exit(-1);
        }
        
        RoutingLayer routingLayer = getRoutingLayerByIndex(layer);
        bool direction = routingLayer.getPreferredDirection();
        Box regionToAdjust = Box(lowerBound, upperBound, -1);
        
        tilesToAdjust = _grid.getBlockedTiles(regionToAdjust, firstTileBox, lastTileBox);
        Grid::TILE &firstTile = tilesToAdjust.first;
        Grid::TILE &lastTile = tilesToAdjust.second;
        
        RoutingTracks routingTracks = getRoutingTracksByIndex(layer);
        DBU trackSpace = routingTracks.getSpace();
        
        int firstTileReduce = _grid.computeTileReduce(regionToAdjust, firstTileBox, trackSpace, true, direction);

        int lastTileReduce = _grid.computeTileReduce(regionToAdjust, lastTileBox, trackSpace, false, direction);
        
        if (direction == RoutingLayer::HORIZONTAL) {                                          // If preferred direction is horizontal, only first and the last line will have specific adjustments
                for (int x = firstTile._x; x <= lastTile._x; x++) {  // Setting capacities of edges completely inside the adjust region according the percentage of reduction
                        for (int y = firstTile._y; y <= lastTile._y; y++) {
                                int edgeCap = _fastRoute.getEdgeCapacity(x, y, layer, x + 1, y, layer);
                                
                                if (y == firstTile._y) {
                                        edgeCap -= firstTileReduce;
                                        if (edgeCap < 0)
                                                edgeCap = 0;
                                        _fastRoute.addAdjustment(x, y, layer, x + 1, y, layer, edgeCap);
                                } else if (y == lastTile._y) {
                                        edgeCap -= lastTileReduce;
                                        if (edgeCap < 0)
                                                edgeCap = 0;
                                        _fastRoute.addAdjustment(x, y, layer, x + 1, y, layer, edgeCap);
                                } else {
                                        edgeCap -= edgeCap*reductionPercentage;
                                        _fastRoute.addAdjustment(x, y, layer, x + 1, y, layer, 0);
                                }
                        }
                }
        } else {                                                   // If preferred direction is vertical, only first and last columns will have specific adjustments
                for (int x = firstTile._x; x <= lastTile._x; x++) {  // Setting capacities of edges completely inside the adjust region according the percentage of reduction
                        for (int y = firstTile._y; y <= lastTile._y; y++) {
                                int edgeCap = _fastRoute.getEdgeCapacity(x, y, layer, x, y + 1, layer);
                                
                                if (x == firstTile._x) {
                                        edgeCap -= firstTileReduce;
                                        if (edgeCap < 0)
                                                edgeCap = 0;
                                        _fastRoute.addAdjustment(x, y, layer, x, y + 1, layer, edgeCap);
                                } else if (x == lastTile._x) {
                                        edgeCap -= lastTileReduce;
                                        if (edgeCap < 0)
                                                edgeCap = 0;
                                        _fastRoute.addAdjustment(x, y, layer, x, y + 1, layer, edgeCap);
                                } else {
                                        edgeCap -= edgeCap*reductionPercentage;
                                        _fastRoute.addAdjustment(x, y, layer, x, y + 1, layer, 0);
                                }
                        }
                }
        }
}

void FastRouteKernel::computeObstaclesAdjustments() {
        std::map<int, std::vector<Box>> obstacles = _grid.getAllObstacles();
        
        for (int layer = 1; layer <= _grid.getNumLayers(); layer++) {
                std::vector<Box> layerObstacles = obstacles[layer];
                if (layerObstacles.size() == 0)
                    continue;
                
                RoutingLayer routingLayer = getRoutingLayerByIndex(layer);
                
                std::pair<Grid::TILE, Grid::TILE> blockedTiles;
                
                bool direction = routingLayer.getPreferredDirection();
                
                std::cout << " > ----Processing " << layerObstacles.size() << 
                             " obstacles in layer " << layer << "\n";
                
                int trackSpace = _grid.getMinWidths()[layer-1];
                
                for (Box& obs : layerObstacles) {
                        Box firstTileBox;
                        Box lastTileBox;
                        
                        blockedTiles = _grid.getBlockedTiles(obs, firstTileBox,
                                                             lastTileBox);
                        
                        Grid::TILE &firstTile = blockedTiles.first;
                        Grid::TILE &lastTile = blockedTiles.second;
                        
                        if (lastTile._x == _grid.getXGrids() - 1 || 
                            lastTile._y == _grid.getYGrids())
                                continue;
                        
                        int firstTileReduce = _grid.computeTileReduce(obs, firstTileBox, trackSpace, true, direction);

                        int lastTileReduce = _grid.computeTileReduce(obs, lastTileBox, trackSpace, false, direction);
                        
                        if (direction == RoutingLayer::HORIZONTAL) {
                                for (int x = firstTile._x; x <= lastTile._x; x++) {  // Setting capacities of completely blocked edges to zero
                                        for (int y = firstTile._y; y <= lastTile._y; y++) {
                                                if (y == firstTile._y) {
                                                        int edgeCap = _fastRoute.getEdgeCapacity(x, y, layer, x + 1, y, layer);
                                                        edgeCap -= firstTileReduce;
                                                        if (edgeCap < 0)
                                                                edgeCap = 0;
                                                        _fastRoute.addAdjustment(x, y, layer, x + 1, y, layer, edgeCap);
                                                } else if (y == lastTile._y) {
                                                        int edgeCap = _fastRoute.getEdgeCapacity(x, y, layer, x + 1, y, layer);
                                                        edgeCap -= lastTileReduce;
                                                        if (edgeCap < 0)
                                                                edgeCap = 0;
                                                        _fastRoute.addAdjustment(x, y, layer, x + 1, y, layer, edgeCap);
                                                } else {
                                                        _fastRoute.addAdjustment(x, y, layer, x + 1, y, layer, 0);
                                                }
                                        }
                                }
                        } else {
                                for (int x = firstTile._x; x <= lastTile._x; x++) {  // Setting capacities of completely blocked edges to zero
                                        for (int y = firstTile._y; y <= lastTile._y; y++) {
                                                if (x == firstTile._x) {
                                                        int edgeCap = _fastRoute.getEdgeCapacity(x, y, layer, x, y + 1, layer);
                                                        edgeCap -= firstTileReduce;
                                                        if (edgeCap < 0)
                                                                edgeCap = 0;
                                                        _fastRoute.addAdjustment(x, y, layer, x, y + 1, layer, edgeCap);
                                                } else if (x == lastTile._x) {
                                                        int edgeCap = _fastRoute.getEdgeCapacity(x, y, layer, x, y + 1, layer);
                                                        edgeCap -= lastTileReduce;
                                                        if (edgeCap < 0)
                                                                edgeCap = 0;
                                                        _fastRoute.addAdjustment(x, y, layer, x, y + 1, layer, edgeCap);
                                                } else {
                                                        _fastRoute.addAdjustment(x, y, layer, x, y + 1, layer, 0);
                                                }
                                        }
                                }
                        }
                }
        }
}

void FastRouteKernel::writeGuides() {
        std::cout << " > Writing guides...\n";
        std::ofstream guideFile;
        guideFile.open(_outfile);
        if (!guideFile.is_open()) {
                std::cout << " > [ERROR] Guides file could not be open!" << std::endl;
                guideFile.close();
                std::exit(1);
        }
        RoutingLayer phLayerF;
        addRemainingGuides(_result);
        
        std::cout << " > Num routed nets: " << _result.size() << "\n";
        int finalLayer;
        for (FastRoute::NET netRoute : _result) {
                guideFile << netRoute.name << "\n";
                guideFile << "(\n";
                std::vector<Box> guideBox;
                finalLayer = -1;
                for (FastRoute::ROUTE route : netRoute.route) {
                       if (route.initLayer != finalLayer && finalLayer != -1) {
                                mergeBox(guideBox);
                                for (Box guide : guideBox){
                                        guideFile << guide.getLowerBound().getX() << " "
                                                  << guide.getLowerBound().getY() << " "
                                                  << guide.getUpperBound().getX() << " "
                                                  << guide.getUpperBound().getY() << " "
                                                  << phLayerF.getName() << "\n";
                                }
                                guideBox.clear();
                                finalLayer = route.initLayer;
                        }
                        if (route.initLayer == route.finalLayer) {
                                if (route.initLayer < _minRoutingLayer && 
                                    route.initX != route.finalX && route.initY != route.finalX) {
                                        std::cout << " > [ERROR] Routing with guides in blocked metal\n"
                                                " > ---- Net: " << netRoute.name << "\n";
                                        std::exit(1);
                                }
                                Box box;
                                box = globalRoutingToBox(route);
                                guideBox.push_back(box);
                                if (route.finalLayer < _minRoutingLayer && !_unidirectionalRoute) {
                                        phLayerF = getRoutingLayerByIndex((route.finalLayer + (_minRoutingLayer - route.finalLayer)));
                                } else {
                                        phLayerF = getRoutingLayerByIndex(route.finalLayer);
                                }
                                finalLayer = route.finalLayer;
                        } else {
                                if (abs(route.finalLayer - route.initLayer) > 1) {
                                        std::cout << " > [ERROR] Connection between"
                                                     "non-adjacent layers";
                                        std::exit(1);
                                } else {
                                        RoutingLayer phLayerI;
                                        if (route.initLayer < _minRoutingLayer && !_unidirectionalRoute) {
                                                phLayerI = getRoutingLayerByIndex((route.initLayer + (_minRoutingLayer - route.initLayer)));
                                        } else {
                                                phLayerI = getRoutingLayerByIndex(route.initLayer);
                                        }
                                        if (route.finalLayer < _minRoutingLayer && !_unidirectionalRoute) {
                                                phLayerF = getRoutingLayerByIndex((route.finalLayer + (_minRoutingLayer - route.finalLayer)));
                                        } else {
                                                phLayerF = getRoutingLayerByIndex(route.finalLayer);
                                        }
                                        finalLayer = route.finalLayer;
                                        Box box;
                                        box = globalRoutingToBox(route);
                                        guideBox.push_back(box);
                                        mergeBox(guideBox);
                                        for (Box guide : guideBox){
                                                 guideFile << guide.getLowerBound().getX() << " "
                                                           << guide.getLowerBound().getY() << " "
                                                           << guide.getUpperBound().getX() << " "
                                                           << guide.getUpperBound().getY() << " "
                                                           << phLayerI.getName() << "\n";
                                        }
                                        guideBox.clear();

                                        box = globalRoutingToBox(route);
                                        guideBox.push_back(box);
                                }
                        }
                }
                mergeBox(guideBox);
                for (Box guide : guideBox){
                         guideFile << guide.getLowerBound().getX() << " "
                                   << guide.getLowerBound().getY() << " "
                                   << guide.getUpperBound().getX() << " "
                                   << guide.getUpperBound().getY() << " "
                                   << phLayerF.getName() << "\n";
                }
                guideFile << ")\n";
        }

        guideFile.close();
        std::cout << " > Writing guides... Done!\n";
}

void FastRouteKernel::printGrid() {
        std::cout << "**** Global Routing Grid ****\n";
        std::cout << "******** Lower left: (" << _grid.getLowerLeftX() << ", " <<
                    _grid.getLowerLeftY() << ") ********\n";
        std::cout << "******** Tile size: " << _grid.getTileWidth() << " ********\n";
        std::cout << "******** xGrids, yGrids: " << _grid.getXGrids() << ", " <<
                    _grid.getYGrids() << " ********\n";
        std::cout << "******** Perfect regular X/Y: " << _grid.isPerfectRegularX() << "/" <<
                    _grid.isPerfectRegularY() << " ********\n";
        std::cout << "******** Num layers: " << _grid.getNumLayers() << " ********\n";
        std::cout << "******** Num nets: " << _netlist.getNetCount() << " ********\n";
        std::cout << "******** Tile size: " << _grid.getPitchesInTile() << "\n";
}

void FastRouteKernel::printHeader() {
        std::cout << "\n";
        std::cout << " *****************\n";
        std::cout << " *   FastRoute   *\n";        
        std::cout << " *****************\n";
        std::cout << "\n";
}

RoutingLayer FastRouteKernel::getRoutingLayerByIndex(int index) {
        RoutingLayer selectedRoutingLayer;
        
        for (RoutingLayer routingLayer : _routingLayers) {
                if (routingLayer.getIndex() == index) {
                        selectedRoutingLayer = routingLayer;
                        break;
                }
        }
        
        return selectedRoutingLayer;
}

RoutingTracks FastRouteKernel::getRoutingTracksByIndex(int layer) {
        RoutingTracks selectedRoutingTracks;
        
        for (RoutingTracks routingTracks: _allRoutingTracks) {
                if (routingTracks.getLayerIndex() == layer) {
                        selectedRoutingTracks = routingTracks;
                }
        }
        
        return selectedRoutingTracks;
}

void FastRouteKernel::addRemainingGuides(std::vector<FastRoute::NET> &globalRoute) {
        std::map<std::string, std::vector<FastRoute::PIN>> allNets;
        allNets = _fastRoute.getNets();
        int localNetsId = allNets.size();

        for (FastRoute::NET &netRoute : globalRoute) {
                std::vector<FastRoute::PIN> &pins = allNets[netRoute.name];

                if (_netsDegree[netRoute.name] < 2) {
                        continue;
                }
                if (netRoute.route.size() == 0) {
                        int lastLayer = -1;
                        for (int p = 0; p < pins.size(); p++){
                                if (p > 0){
                                        if (pins[p].x != pins[p-1].x ||
                                                pins[p].y != pins[p-1].y){
                                                std::cout << " > [ERROR] Net " << netRoute.name << " not properly covered.";
                                                exit(-1);
                                        }
                                }
                                if (pins[p].layer > lastLayer)
                                        lastLayer = pins[p].layer;
                        }

                        for (int l = _minRoutingLayer - _fixLayer; l <= lastLayer; l++) {
                                FastRoute::ROUTE route;
                                route.initLayer = l;
                                route.initX = pins[0].x;
                                route.initY = pins[0].y;
                                route.finalLayer = l + 1;
                                route.finalX = pins[0].x;
                                route.finalY = pins[0].y;
                                netRoute.route.push_back(route);
                        }
                } else {
                        for (FastRoute::PIN pin : pins) {
                                if (pin.layer > 1) {
                                        for (int l = _minRoutingLayer - _fixLayer; l <= pin.layer - 1; l++) {
                                                FastRoute::ROUTE route;
                                                route.initLayer = l;
                                                route.initX = pin.x;
                                                route.initY = pin.y;
                                                route.finalLayer = l + 1;
                                                route.finalX = pin.x;
                                                route.finalY = pin.y;
                                                netRoute.route.push_back(route);
                                        }
                                }
                        }
                }
                allNets.erase(netRoute.name);
        }

        if (allNets.size() > 0) {
                for (std::map<std::string, std::vector<FastRoute::PIN>>::iterator
                         it = allNets.begin();
                     it != allNets.end(); ++it) {
                        std::vector<FastRoute::PIN> &pins = it->second;

                        FastRoute::NET localNet;
                        localNet.id = localNetsId;
                        localNetsId++;
                        localNet.name = it->first;
                        for (FastRoute::PIN pin : pins) {
                                FastRoute::ROUTE route;
                                route.initLayer = pin.layer;
                                route.initX = pin.x;
                                route.initY = pin.y;
                                route.finalLayer = pin.layer;
                                route.finalX = pin.x;
                                route.finalY = pin.y;
                                localNet.route.push_back(route);
                        }
                        globalRoute.push_back(localNet);
                }
        }
}

void FastRouteKernel::mergeBox(std::vector<Box>& guideBox) {
        std::vector<Box> finalBox;
        if (guideBox.size() < 1) {
                std::cout << " > [ERROR] Guides vector is empty!!!\n";
                std::exit(1);
        }
        finalBox.push_back(guideBox[0]);
        for (int i=1; i < guideBox.size(); i++){
                Box box = guideBox[i];
                Box & lastBox = finalBox.back();
                if (lastBox.overlap(box)) {
                        DBU lowerX = std::min(lastBox.getLowerBound().getX(),
                                              box.getLowerBound().getX());
                        DBU lowerY = std::min(lastBox.getLowerBound().getY(),
                                              box.getLowerBound().getY());
                        DBU upperX = std::max(lastBox.getUpperBound().getX(),
                                              box.getUpperBound().getX());
                        DBU upperY = std::max(lastBox.getUpperBound().getY(),
                                              box.getUpperBound().getY());
                        lastBox = Box(lowerX, lowerY, upperX, upperY, -1);
                } else
                        finalBox.push_back(box);
        } 
        guideBox.clear();
        guideBox = finalBox;
}

Box FastRouteKernel::globalRoutingToBox(const FastRoute::ROUTE &route) {
        Box dieBounds = Box(_grid.getLowerLeftX(), _grid.getLowerLeftY(),
                            _grid.getUpperRightX(), _grid.getUpperRightY(),
                            -1);
        long initX, initY;
        long finalX, finalY;

        if (route.initX < route.finalX) {
                initX = route.initX;
                finalX = route.finalX;
        } else {
                initX = route.finalX;
                finalX = route.initX;
        }

        if (route.initY < route.finalY) {
                initY = route.initY;
                finalY = route.finalY;
        } else {
                initY = route.finalY;
                finalY = route.initY;
        }

        DBU llX = initX - (_grid.getTileWidth() / 2);
        DBU llY = initY - (_grid.getTileHeight() / 2);

        DBU urX = finalX + (_grid.getTileWidth() / 2);
        DBU urY = finalY + (_grid.getTileHeight() / 2);

        if ((dieBounds.getUpperBound().getX() - urX) / _grid.getTileWidth() < 1) {
                urX = dieBounds.getUpperBound().getX();
        }
        if ((dieBounds.getUpperBound().getY() - urY) / _grid.getTileHeight() < 1) {
                urY = dieBounds.getUpperBound().getY();
        }

        Coordinate lowerLeft = Coordinate(llX, llY);
        Coordinate upperRight = Coordinate(urX, urY);

        Box routeBds = Box(lowerLeft, upperRight, -1);
        return routeBds;
}

void FastRouteKernel::checkPinPlacement() {
        bool invalid = false;
        std::map<int, std::vector<Coordinate>> mapLayerToPositions;
        
        for (Pin port : _netlist.getAllPorts()) {
                if (port.getNumLayers() == 0) {
                        std::cout << "[ERROR] Pin " << port.getName() << " does "
                            "not have layer assignment\n";
                        exit(1);
                }
                DBU layer = port.getLayers()[0]; // port have only one layer
                
                if (mapLayerToPositions[layer].size() == 0) {
                        mapLayerToPositions[layer].push_back(port.getPosition());
                        continue;
                }
                
                for (Coordinate pos : mapLayerToPositions[layer]) {
                        if (pos == port.getPosition()) {
                                std::cout << "[ERROR] At least 2 pins in position ("
                                          << pos.getX() << ", " << pos.getY()
                                          << "), layer " << layer+1 << "\n";
                                invalid = true;
                        }
                }
                mapLayerToPositions[layer].push_back(port.getPosition());
        }
        
        if (invalid) {
                std::exit(-1);
        }
}

}
