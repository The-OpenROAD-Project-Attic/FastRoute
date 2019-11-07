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

#include "FastRouteKernel.h"

FastRouteKernel::FastRouteKernel(Parameters& parms)
    : _parms(&parms), _dbWrapper(_netlist, _grid, parms) {
        _adjustment = _parms->getAdjustment();
        _minRoutingLayer = _parms->getMinRoutingLayer();
        _maxRoutingLayer = _parms->getMaxRoutingLayer();
        _unidirectionalRoute = _parms->getUnidirectionalRoute();
        _interactiveMode = _parms->isInteractiveMode();
}

void FastRouteKernel::initGrid() {        
        _dbWrapper.initGrid();
        _dbWrapper.computeCapacities();
        _dbWrapper.computeSpacingsAndMinWidth();
        
        _fastRoute.setLowerLeft(_grid.getLowerLeftX(), _grid.getLowerLeftY());
        _fastRoute.setTileSize(_grid.getTileWidth(), _grid.getTileHeight());
        _fastRoute.setGridsAndLayers(_grid.getXGrids(), _grid.getYGrids(), _grid.getNumLayers());
        _fastRoute.setLayerOrientation(_grid.getMetal1Orientation());
}

void FastRouteKernel::setCapacities() {
        for (int l = 1; l <= _grid.getNumLayers(); l++) {
                _fastRoute.addHCapacity(_grid.getHorizontalEdgesCapacities()[l-1], l);
                _fastRoute.addVCapacity(_grid.getVerticalEdgesCapacities()[l-1], l);
                
                _hCapacities.push_back(_grid.getHorizontalEdgesCapacities()[l-1]);
                _vCapacities.push_back(_grid.getVerticalEdgesCapacities()[l-1]);
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
        int idx = 0;
        _fastRoute.setNumberNets(_netlist.getNetCount());
        _fastRoute.setMaxNetDegree(_netlist.getMaxNetDegree());
        
        for (Net net : _netlist.getNets()) {
                if (net.getNumPins() == 1) {
                        continue;
                }
                
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
                
                _fastRoute.addNet(netName, idx, pins.size(), 1, grPins);
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
                
                if (layer < _minRoutingLayer || layer > _maxRoutingLayer &&
                    _maxRoutingLayer > 0)
                        continue;

                int newVCapacity = 0;
                int newHCapacity = 0;
                
                if (_grid.getMetal1Orientation() == Grid::HORIZONTAL && layer%2 == 1) {
                        hSpace = _grid.getMinWidths()[layer-1];
                        newHCapacity = std::floor((_grid.getTileHeight() + yExtra)/hSpace);
                } else if (_grid.getMetal1Orientation() == Grid::HORIZONTAL && layer%2 == 0) {
                        vSpace = _grid.getMinWidths()[layer-1];
                        newVCapacity = std::floor((_grid.getTileWidth() + xExtra)/vSpace);
                } else if (_grid.getMetal1Orientation() == Grid::VERTICAL && layer%2 == 0) {
                        hSpace = _grid.getMinWidths()[layer-1];
                        newHCapacity = std::floor((_grid.getTileHeight() + yExtra)/hSpace);
                } else if (_grid.getMetal1Orientation() == Grid::VERTICAL && layer%2 == 1) {
                        vSpace = _grid.getMinWidths()[layer-1];
                        newVCapacity = std::floor((_grid.getTileWidth() + xExtra)/vSpace);
                } else {
                    std::cout << "[ERROR] Layer spacing not found. Exiting...\n";
                    std::exit(0);
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

void FastRouteKernel::computeUserAdjustments() {
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
        std::cout << "******** Tile size: " << _parms->getPitchesInTile() << "\n";
}

void FastRouteKernel::run() {
        _parms->printAll();
        if (!_interactiveMode) {
                std::cout << "Parsing input files...\n";
                _dbWrapper.parseLEF(_parms->getInputLefFile()); 
                _dbWrapper.parseDEF(_parms->getInputDefFile());
                std::cout << "Parsing input files... Done!\n";
        }
        
        std::cout << "Initializing grid...\n";
        initGrid();
        std::cout << "Initializing grid... Done!\n";
        
        std::cout << "Setting capacities...\n";
        setCapacities();
        std::cout << "Setting capacities... Done!\n";
        
        std::cout << "Setting spacings and widths...\n";
        setSpacingsAndMinWidths();
        std::cout << "Setting spacings and widths... Done!\n";
        
        std::cout << "Initializing nets...\n";
        initializeNets();
        std::cout << "Initializing nets... Done!\n";
        
        std::cout << "Adjusting grid...\n";
        computeGridAdjustments();
        std::cout << "Adjusting grid... Done!\n";
        
        std::cout << "Computing user defined adjustments...\n";
        computeUserAdjustments();
        std::cout << "Computing user defined adjustments... Done!\n";
        
        _fastRoute.initAuxVar();

        std::cout << "Running FastRoute...\n";
        _fastRoute.run(_result);
        std::cout << "Running FastRoute... Done!\n";
}