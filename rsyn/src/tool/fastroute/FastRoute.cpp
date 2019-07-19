////////////////////////////////////////////////////////////////////////////////
// Authors: Vitor Bandeira, Eder Matheus Monteiro e Isadora
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

#include "FastRoute.h"
#include "phy/PhysicalDesign.h"
#include "phy/obj/decl/PhysicalDesign.h"
#include "phy/obj/decl/PhysicalLayer.h"
#include "phy/util/PhysicalTypes.h"
#include "util/Bounds.h"
#include "util/DBU.h"
#include "phy/PhysicalRouting.h"
#include "core/RsynTypes.h"
#include "util/Stepwatch.h"
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <istream>
#include <utility>
#include <math.h>
#include <vector>

namespace Rsyn {

bool FastRouteProcess::run(const Rsyn::Json &params) {
        Stepwatch watch("FastRoute...");
        std::vector<FastRoute::NET> result;
        std::string outfile = params.value("outfile", "out.guide");
        
        adjustment = params.value("adjustment", 0.0);
        maxRoutingLayer = params.value("maxRoutingLayer", -1);
        design = session.getDesign();
        module = design.getTopModule();
        phDesign = session.getPhysicalDesign();
        
        std::cout << "\n----------------\n";
        std::cout << "Params: \n";
        std::cout << "**** Output file: " << outfile << "\n";
        std::cout << "**** Capacity adjustment: " << adjustment << "\n";
        std::cout << "**** Max routing layer: " << maxRoutingLayer << "\n";
        std::cout << "\n----------------\n";
        
        std::cout << "Initing grid...\n";
        initGrid();
        std::cout << "Initing grid... Done!\n";

        std::cout << "Setting capacities...\n";
        setCapacities();
        std::cout << "Setting capacities... Done!\n";

        std::cout << "Setting layer direction...\n";
        setLayerDirection();
        std::cout << "Setting layer direction... Done!\n";

        std::cout << "Setting spacings and widths...\n";
        setSpacingsAndMinWidth();
        std::cout << "Setting spacings and widths... Done!\n";

        std::cout << "Initing nets...\n";
        initNets();
        std::cout << "Initing nets... Done!\n";

        std::cout << "Adjusting grid...\n";
        setGridAdjustments();
        std::cout << "Adjusting grid... Done!\n";

        std::cout << "Computing obstacles adjustments...\n";
        computeObstaclesAdjustments();
        std::cout << "Computing obstacles adjustments... Done!\n";

        std::cout << "Computing user defined adjustments...\n";
        computeSimpleAdjustments();
        std::cout << "Computing user defined adjustments... Done!\n";
        
        fastRoute.initAuxVar();

        std::cout << "Running FastRoute...\n";
        fastRoute.run(result);
        std::cout << "Running FastRoute... Done!\n";

        std::cout << "Writing guides...\n";
        writeGuides(result, outfile);
        std::cout << "Writing guides... Done!\n";

        std::cout << "Writing est...\n";
        writeEst(result, outfile);
        std::cout << "Writing est... Done!\n";

        return 0;
}

void FastRouteProcess::initGrid() {
        int nLayers = 0;
        DBU trackSpacing;

        for (Rsyn::PhysicalLayer phLayer : phDesign.allPhysicalLayers()) {
                if (phLayer.getType() != Rsyn::ROUTING)
                        continue;

                nLayers++;
                if (phLayer.getRelativeIndex() != selectedMetal - 1)
                        continue;

                Rsyn::PhysicalLayerDirection metalDirection = phLayer.getDirection();

                for (Rsyn::PhysicalTracks phTrack : phDesign.allPhysicalTracks(phLayer)) {
                        if (phTrack.getDirection() == (PhysicalTrackDirection)metalDirection)
                                continue;
                        trackSpacing = phTrack.getSpace();
                        break;
                }
        }

        DBU tileSize = trackSpacing * pitchesInTile;

        Rsyn::PhysicalDie phDie = phDesign.getPhysicalDie();
        Bounds dieBounds = phDie.getBounds();

        DBU dieX = dieBounds[UPPER][X] - dieBounds[LOWER][X];
        DBU dieY = dieBounds[UPPER][Y] - dieBounds[LOWER][Y];

        int xGrid = std::ceil((float)dieX / tileSize);
        int yGrid = std::ceil((float)dieY / tileSize);
        
        if ((xGrid*tileSize) == dieX)
                grid.perfect_regular_x = true;
        
        if ((yGrid*tileSize) == dieY)
                grid.perfect_regular_y = true;

        fastRoute.setLowerLeft(dieBounds[LOWER][X], dieBounds[LOWER][Y]);
        fastRoute.setTileSize(tileSize, tileSize);
        fastRoute.setGridsAndLayers(xGrid, yGrid, nLayers);

        grid.lower_left_x = dieBounds[LOWER][X];
        grid.lower_left_y = dieBounds[LOWER][Y];
        grid.tile_width = tileSize;
        grid.tile_height = tileSize;
        grid.yGrids = yGrid;
        grid.xGrids = xGrid;
}

void FastRouteProcess::setCapacities() {
        for (PhysicalLayer phLayer : phDesign.allPhysicalLayers()) {
                int vCapacity = 0;
                int hCapacity = 0;

                if (phLayer.getType() != Rsyn::ROUTING)
                        continue;

                if (phLayer.getDirection() == Rsyn::HORIZONTAL) {
                        for (PhysicalTracks phTracks : phDesign.allPhysicalTracks(phLayer)) {
                                if (phTracks.getDirection() != (PhysicalTrackDirection)Rsyn::HORIZONTAL) {
                                        hCapacity = std::floor((float)grid.tile_width / phTracks.getSpace());
                                        break;
                                }
                        }

                        if (phLayer.getRelativeIndex() >= maxRoutingLayer && maxRoutingLayer > 0)
                                hCapacity = 0;
                        
                        fastRoute.addVCapacity(0, phLayer.getRelativeIndex() + 1);
                        fastRoute.addHCapacity(hCapacity, phLayer.getRelativeIndex() + 1);

                        vCapacities.push_back(0);
                        hCapacities.push_back(hCapacity);
                } else {
                        for (PhysicalTracks phTracks : phDesign.allPhysicalTracks(phLayer)) {
                                if (phTracks.getDirection() != (PhysicalTrackDirection)Rsyn::VERTICAL) {
                                        vCapacity = std::floor((float)grid.tile_width / phTracks.getSpace());
                                        break;
                                }
                        }
                        
                        if (phLayer.getRelativeIndex() >= maxRoutingLayer && maxRoutingLayer > 0)
                                vCapacity = 0;

                        fastRoute.addVCapacity(vCapacity, phLayer.getRelativeIndex() + 1);
                        fastRoute.addHCapacity(0, phLayer.getRelativeIndex() + 1);

                        vCapacities.push_back(vCapacity);
                        hCapacities.push_back(0);
                }
        }
}

void FastRouteProcess::setLayerDirection() {
        for (PhysicalLayer phLayer : phDesign.allPhysicalLayers()) {
                if (phLayer.getType() != Rsyn::ROUTING)
                        continue;

                if (phLayer.getRelativeIndex() != 0)
                        continue;

                if (phLayer.getDirection() == Rsyn::HORIZONTAL) {
                        fastRoute.setLayerOrientation(0);
                } else {
                        fastRoute.setLayerOrientation(1);
                }
                break;
        }
}

void FastRouteProcess::setSpacingsAndMinWidth() {
        int minSpacing = 0;
        int minWidth;

        for (PhysicalLayer phLayer : phDesign.allPhysicalLayers()) {
                if (phLayer.getType() != Rsyn::ROUTING)
                        continue;

                for (PhysicalTracks phTracks : phDesign.allPhysicalTracks(phLayer)) {
                        if (phTracks.getDirection() != (PhysicalTrackDirection)phLayer.getDirection())
                                minWidth = phTracks.getSpace();
                        else
                                continue;
                        fastRoute.addMinSpacing(minSpacing, phLayer.getRelativeIndex() + 1);
                        fastRoute.addMinWidth(minWidth, phLayer.getRelativeIndex() + 1);
                        fastRoute.addViaSpacing(1, phLayer.getRelativeIndex() + 1);
                }
        }
}

void FastRouteProcess::initNets() {
        int idx = 0;
        int numNets = 0;

        for (Rsyn::Net net : module.allNets()) {
                numNets++;
        }

        fastRoute.setNumberNets(numNets);

        for (Rsyn::Net net : module.allNets()) {
                if (net.getNumPins() == 1)
                        continue;

                if (net.getUse() == Rsyn::POWER || net.getUse() == Rsyn::GROUND)
                        continue;

                netsDegree[net.getName()] = net.getNumPins();

                std::vector<FastRoute::PIN> pins;
                for (Rsyn::Pin pin : net.allPins()) {
                        DBUxy pinPosition;
                        int pinLayer;
                        int numOfLayers;
                        if (pin.getInstanceType() == Rsyn::CELL) {
                                Rsyn::PhysicalLibraryPin phLibPin = phDesign.getPhysicalLibraryPin(pin);
                                Rsyn::Cell cell = pin.getInstance().asCell();
                                Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
                                const DBUxy cellPos = phCell.getPosition();
                                const Rsyn::PhysicalTransform &transform = phCell.getTransform(true);

                                for (Rsyn::PhysicalPinGeometry pinGeo : phLibPin.allPinGeometries()) {
                                        if (!pinGeo.hasPinLayer())
                                                continue;
                                        numOfLayers = pinGeo.allPinLayers().size();
                                        for (Rsyn::PhysicalPinLayer phPinLayer : pinGeo.allPinLayers()) {
                                                pinLayer = phPinLayer.getLayer().getRelativeIndex();

                                                std::vector<DBUxy> pinBdsPositions;
                                                DBUxy bdsPosition;
                                                for (Bounds bds : phPinLayer.allBounds()) {
                                                        bds = transform.apply(bds);
                                                        bds.translate(cellPos);
                                                        bdsPosition = bds.computeCenter();
                                                        getPosOnGrid(bdsPosition);
                                                        pinBdsPositions.push_back(bdsPosition);
                                                }

                                                int mostVoted = 0;

                                                for (DBUxy pos : pinBdsPositions) {
                                                        int equals = std::count(pinBdsPositions.begin(),
                                                                                pinBdsPositions.end(), pos);
                                                        if (equals > mostVoted) {
                                                                pinPosition = pos;
                                                                mostVoted = equals;
                                                        }
                                                }
                                        }
                                }
                        } else if (pin.getInstanceType() == Rsyn::PORT) {
                                Rsyn::PhysicalLibraryPin phLibPin = phDesign.getPhysicalLibraryPin(pin);
                                Rsyn::Port port = pin.getInstance().asPort();
                                Rsyn::PhysicalPort phPort = phDesign.getPhysicalPort(port);
                                DBUxy portPos = phPort.getPosition();
                                getPosOnGrid(portPos);
                                pinPosition = portPos;
                                pinLayer = phPort.getLayer().getRelativeIndex();
                                numOfLayers = 1;
                        }
                        for (int l = numOfLayers - 1; l >= 0; l--) {
                                FastRoute::PIN grPin;
                                grPin.x = pinPosition.x;
                                grPin.y = pinPosition.y;
                                grPin.layer = (pinLayer - l) + 1;
                                pins.push_back(grPin);
                        }
                }
                FastRoute::PIN grPins[pins.size()];

                char netName[net.getName().size() + 1];
                strcpy(netName, net.getName().c_str());
                int count = 0;
                for (FastRoute::PIN pin : pins) {
                        grPins[count] = pin;
                        count++;
                }

                fastRoute.addNet(netName, idx, pins.size(), 1, grPins);
                idx++;
        }

        fastRoute.initEdges();
}

void FastRouteProcess::setGridAdjustments() {
        grid.perfect_regular_x = false;
        grid.perfect_regular_y = false;

        Rsyn::PhysicalDie phDie = phDesign.getPhysicalDie();
        Bounds dieBounds = phDie.getBounds();
        DBUxy upperDieBounds = dieBounds[UPPER];

        int xGrids = grid.xGrids;
        int yGrids = grid.yGrids;
        int xBlocked = upperDieBounds.x % xGrids;
        int yBlocked = upperDieBounds.y % yGrids;
        float percentageBlockedX = xBlocked / grid.tile_width;
        float percentageBlockedY = yBlocked / grid.tile_height;

        for (Rsyn::PhysicalLayer phLayer : phDesign.allPhysicalLayers()) {
                if (phLayer.getType() != Rsyn::ROUTING)
                        continue;

                int layerN = phLayer.getRelativeIndex() + 1;
                int newVCapacity = std::floor((float)vCapacities[layerN - 1] * percentageBlockedX);
                int newHCapacity = std::floor((float)hCapacities[layerN - 1] * percentageBlockedY);

                int numAdjustments = 0;
                for (int i = 1; i < yGrids; i++)
                        numAdjustments++;
                for (int i = 1; i < xGrids; i++)
                        numAdjustments++;
                fastRoute.setNumAdjustments(numAdjustments);

                if (!grid.perfect_regular_x) {
                        for (int i = 1; i < yGrids; i++) {
                                fastRoute.addAdjustment(xGrids - 1, i - 1, layerN, xGrids - 1, i, layerN, newVCapacity);
                        }
                }
                if (!grid.perfect_regular_y) {
                        for (int i = 1; i < xGrids; i++) {
                                fastRoute.addAdjustment(i - 1, yGrids - 1, layerN, i, yGrids - 1, layerN, newHCapacity);
                        }
                }
        }
}

void FastRouteProcess::computeSimpleAdjustments() {
        // Temporary adjustment: fixed percentage per layer
        if (adjustment == 0.0)
                return;
        
        int xGrids = grid.xGrids;
        int yGrids = grid.yGrids;
        int numAdjustments = 0;

        float percentageBlockedX = adjustment;
        float percentageBlockedY = adjustment;

        for (Rsyn::PhysicalLayer phLayer : phDesign.allPhysicalLayers()) {
                if (phLayer.getType() != Rsyn::ROUTING)
                        continue;

                for (int y = 0; y < yGrids; y++) {
                        for (int x = 0; x < xGrids; x++) {
                                numAdjustments++;
                        }
                }
        }

        numAdjustments *= 2;
        fastRoute.setNumAdjustments(numAdjustments);

        for (Rsyn::PhysicalLayer phLayer : phDesign.allPhysicalLayers()) {
                if (phLayer.getType() != Rsyn::ROUTING)
                        continue;

                int layerN = phLayer.getRelativeIndex() + 1;

                if (hCapacities[layerN - 1] != 0) {
                        for (int y = 1; y < yGrids; y++) {
                                for (int x = 1; x < xGrids; x++) {
                                        int edgeCap = fastRoute.getEdgeCapacity(x - 1, y - 1, layerN, x, y - 1, layerN);
                                        int newHCapacity = std::floor((float)edgeCap * (1 - percentageBlockedY));
                                        fastRoute.addAdjustment(x - 1, y - 1, layerN, x, y - 1, layerN, newHCapacity);
                                }
                        }
                }

                if (vCapacities[layerN - 1] != 0) {
                        for (int x = 1; x < xGrids; x++) {
                                for (int y = 1; y < yGrids; y++) {
                                        int edgeCap = fastRoute.getEdgeCapacity(x - 1, y - 1, layerN, x - 1, y, layerN);
                                        int newVCapacity = std::floor((float)edgeCap * (1 - percentageBlockedX));
                                        fastRoute.addAdjustment(x - 1, y - 1, layerN, x - 1, y, layerN, newVCapacity);
                                }
                        }
                }
        }
}

void FastRouteProcess::computeObstaclesAdjustments() {
        std::map<int, std::vector<Bounds>> mapLayerObstacles;
        for (Rsyn::Instance inst : module.allInstances()) {
                if (inst.getType() != Rsyn::CELL)
                        continue;
                Rsyn::Cell cell = inst.asCell();
                Rsyn::PhysicalLibraryCell phLibCell = phDesign.getPhysicalLibraryCell(cell);
                if (!phLibCell.hasObstacles())
                        continue;
                Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
                const Rsyn::PhysicalTransform &transform = phCell.getTransform(true);
                DBUxy pos = phCell.getPosition();
                Rsyn::PhysicalLayer upperBlockLayer;

                for (Rsyn::PhysicalObstacle phObs : phLibCell.allObstacles()) {
                        Rsyn::PhysicalLayer phLayer = phObs.getLayer();
                        if (upperBlockLayer == nullptr) {
                                upperBlockLayer = phLayer;
                        } else if (phLayer.getIndex() > upperBlockLayer.getIndex()) {
                                upperBlockLayer = phLayer;
                        }
                }

                for (Rsyn::PhysicalObstacle phObs : phLibCell.allObstacles()) {
                        Rsyn::PhysicalLayer phLayer = phObs.getLayer();
                        for (Bounds bds : phObs.allBounds()) {
                                bds = transform.apply(bds);
                                bds.translate(pos);
                                mapLayerObstacles[phLayer.getRelativeIndex()].push_back(bds);
                        }  // end for
                }          // end for
        }

        getSpecialNetsObstacles(mapLayerObstacles);

        for (std::map<int, std::vector<Bounds>>::iterator it = mapLayerObstacles.begin(); it != mapLayerObstacles.end(); ++it) {
                std::cout << "----Processing " << it->second.size() << " obstacles in Metal" << it->first + 1 << "\n";
                Rsyn::PhysicalLayer phLayer = phDesign.getPhysicalLayerByIndex(Rsyn::ROUTING, it->first);
                bool horizontal = phLayer.getDirection() == Rsyn::HORIZONTAL ? 1 : 0;
                std::pair<TILE, TILE> blockedTiles;

                DBU trackSpace;
                for (PhysicalTracks phTracks : phDesign.allPhysicalTracks(phLayer)) {
                        if (horizontal) {
                                if (phTracks.getDirection() != (PhysicalTrackDirection)Rsyn::HORIZONTAL) {
                                        trackSpace = phTracks.getSpace();
                                        break;
                                }
                        } else {
                                if (phTracks.getDirection() != (PhysicalTrackDirection)Rsyn::VERTICAL) {
                                        trackSpace = phTracks.getSpace();
                                        break;
                                }
                        }
                }

                for (int b = 0; b < it->second.size(); b++) {
                        Bounds &obs = it->second[b];
                        Bounds firstTileBds;
                        Bounds lastTileBds;

                        blockedTiles = getBlockedTiles(obs, firstTileBds, lastTileBds);
                        TILE &firstTile = blockedTiles.first;
                        TILE &lastTile = blockedTiles.second;

                        if (lastTile.x == grid.xGrids - 1 || lastTile.y == grid.yGrids)
                                continue;

                        int firstTileReduce = computeTileReduce(obs, firstTileBds, trackSpace, true, horizontal);

                        int lastTileReduce = computeTileReduce(obs, lastTileBds, trackSpace, false, horizontal);

                        if (horizontal) {                                          // If preferred direction is horizontal, only first and the last line will have specific adjustments
                                for (int x = firstTile.x; x <= lastTile.x; x++) {  // Setting capacities of completely blocked edges to zero
                                        for (int y = firstTile.y; y <= lastTile.y; y++) {
                                                if (y == firstTile.y) {
                                                        int edgeCap = fastRoute.getEdgeCapacity(x, y, phLayer.getRelativeIndex() + 1, x + 1, y, phLayer.getRelativeIndex() + 1);
                                                        edgeCap -= firstTileReduce;
                                                        if (edgeCap < 0)
                                                                edgeCap = 0;
                                                        fastRoute.addAdjustment(x, y, phLayer.getRelativeIndex() + 1,
                                                                                x + 1, y, phLayer.getRelativeIndex() + 1, edgeCap);
                                                } else if (y == lastTile.y) {
                                                        int edgeCap = fastRoute.getEdgeCapacity(x, y, phLayer.getRelativeIndex() + 1, x + 1, y, phLayer.getRelativeIndex() + 1);
                                                        edgeCap -= lastTileReduce;
                                                        if (edgeCap < 0)
                                                                edgeCap = 0;
                                                        fastRoute.addAdjustment(x, y, phLayer.getRelativeIndex() + 1,
                                                                                x + 1, y, phLayer.getRelativeIndex() + 1, edgeCap);
                                                } else {
                                                        fastRoute.addAdjustment(x, y, phLayer.getRelativeIndex() + 1,
                                                                                x + 1, y, phLayer.getRelativeIndex() + 1, 0);
                                                }
                                        }
                                }
                        } else {                                                   // If preferred direction is vertical, only first and last columns will have specific adjustments
                                for (int x = firstTile.x; x <= lastTile.x; x++) {  // Setting capacities of completely blocked edges to zero
                                        for (int y = firstTile.y; y <= lastTile.y; y++) {
                                                if (x == firstTile.x) {
                                                        int edgeCap = fastRoute.getEdgeCapacity(x, y, phLayer.getRelativeIndex() + 1, x, y + 1, phLayer.getRelativeIndex() + 1);
                                                        edgeCap -= firstTileReduce;
                                                        if (edgeCap < 0)
                                                                edgeCap = 0;
                                                        fastRoute.addAdjustment(x, y, phLayer.getRelativeIndex() + 1,
                                                                                x, y + 1, phLayer.getRelativeIndex() + 1, edgeCap);
                                                } else if (x == lastTile.x) {
                                                        int edgeCap = fastRoute.getEdgeCapacity(x, y, phLayer.getRelativeIndex() + 1, x, y + 1, phLayer.getRelativeIndex() + 1);
                                                        edgeCap -= lastTileReduce;
                                                        if (edgeCap < 0)
                                                                edgeCap = 0;
                                                        fastRoute.addAdjustment(x, y, phLayer.getRelativeIndex() + 1,
                                                                                x, y + 1, phLayer.getRelativeIndex() + 1, edgeCap);
                                                } else {
                                                        fastRoute.addAdjustment(x, y, phLayer.getRelativeIndex() + 1,
                                                                                x, y + 1, phLayer.getRelativeIndex() + 1, 0);
                                                }
                                        }
                                }
                        }
                }
        }
}

void FastRouteProcess::writeGuides(std::vector<FastRoute::NET> &globalRoute, std::string filename) {
        std::ofstream guideFile;
        guideFile.open(filename);
        if (!guideFile.is_open()) {
                std::cout << "Error in writeFile!" << std::endl;
                guideFile.close();
                std::exit(0);
        }

        addRemainingGuides(globalRoute);

        std::cout << "Num routed nets: " << globalRoute.size() << "\n";
        Rsyn::PhysicalLayer phLayerF; 
        int finalLayer;
        for (FastRoute::NET netRoute : globalRoute) {
                guideFile << netRoute.name << "\n";
                guideFile << "(\n";
                std::vector<Bounds> guideBds;
                finalLayer = -1;
                for (FastRoute::ROUTE route : netRoute.route) {
                       if (route.initLayer != finalLayer && finalLayer != -1) {
                                mergeBounds(guideBds);
                                for (Bounds guide : guideBds){
                                        guideFile << guide.getLower().x << " "
                                                  << guide.getLower().y << " "
                                                  << guide.getUpper().x << " "
                                                  << guide.getUpper().y << " "
                                                  << phLayerF.getName() << "\n";
                                }
                                guideBds.clear();
                                finalLayer = route.initLayer;
                        }
                        if (route.initLayer == route.finalLayer) {
                                Bounds bds;
                                bds = globalRoutingToBounds(route);
                                guideBds.push_back(bds);
                                phLayerF =
                                    phDesign.getPhysicalLayerByIndex(Rsyn::ROUTING, route.finalLayer - 1);
                                finalLayer = route.finalLayer;
                        } else {
                                if (abs(route.finalLayer - route.initLayer) > 1) {
                                        std::cout << "Error: connection between"
                                                     "non-adjacent layers";
                                        std::exit(0);
                                } else {
                                        Rsyn::PhysicalLayer phLayerI =
                                            phDesign.getPhysicalLayerByIndex(Rsyn::ROUTING, route.initLayer - 1);
                                        phLayerF =
                                            phDesign.getPhysicalLayerByIndex(Rsyn::ROUTING, route.finalLayer - 1);
                                        finalLayer = route.finalLayer;
                                        Bounds bds;
                                        bds = globalRoutingToBounds(route);
                                        guideBds.push_back(bds);
                                        mergeBounds(guideBds);
                                        for (Bounds guide : guideBds){
                                                 guideFile << guide.getLower().x << " "
                                                           << guide.getLower().y << " "
                                                           << guide.getUpper().x << " "
                                                           << guide.getUpper().y << " "
                                                           << phLayerI.getName() << "\n";
                                        }
                                        guideBds.clear();

                                        bds = globalRoutingToBounds(route);
                                        guideBds.push_back(bds);
                                }
                        }
                }
                mergeBounds(guideBds);
                for (Bounds guide : guideBds){
                         guideFile << guide.getLower().x << " "
                                   << guide.getLower().y << " "
                                   << guide.getUpper().x << " "
                                   << guide.getUpper().y << " "
                                   << phLayerF.getName() << "\n";
                }
                guideFile << ")\n";
        }

        guideFile.close();
}

void FastRouteProcess::mergeBounds(std::vector<Bounds> & guideBds){
       std::vector<Bounds> finalBds;
       finalBds.push_back(guideBds[0]);
       for (int i=1; i < guideBds.size(); i++){
               Bounds bds = guideBds[i];
               Bounds & lastBds = finalBds.back();
               if (lastBds.overlap(bds)){
                      lastBds[LOWER][X] = std::min(lastBds[LOWER][X], bds[LOWER][X]); 
                      lastBds[LOWER][Y] = std::min(lastBds[LOWER][Y], bds[LOWER][Y]); 
                      lastBds[UPPER][X] = std::max(lastBds[UPPER][X], bds[UPPER][X]); 
                      lastBds[UPPER][Y] = std::max(lastBds[UPPER][Y], bds[UPPER][Y]); 
               } else
                      finalBds.push_back(bds); 
       } 
       guideBds.clear();
       guideBds = finalBds;
}

void FastRouteProcess::writeEst(const std::vector<FastRoute::NET> &globalRoute, std::string filename) {
        std::ofstream estFile;
        estFile.open(filename + ".est");

        if (!estFile.is_open()) {
                std::cout << "Error in writeFile!" << std::endl;
                estFile.close();
                std::exit(0);
        }

        for (FastRoute::NET netRoute : globalRoute) {
                estFile << netRoute.name << " " << netRoute.id << " " << netRoute.route.size() << "\n";
                for (FastRoute::ROUTE route : netRoute.route) {
                        estFile << "(" << route.initX << "," << route.initY << "," << route.initLayer << ")-(" << route.finalX << "," << route.finalY << "," << route.finalLayer << ")\n";
                }
                estFile << "!\n";
        }

        estFile.close();
}

void FastRouteProcess::getPosOnGrid(DBUxy &pos) {
        DBU x = pos.x;
        DBU y = pos.y;

        // Computing x and y center:
        int gCellId_X = floor((float)((x - grid.lower_left_x) / grid.tile_width));
        int gCellId_Y = floor((float)((y - grid.lower_left_y) / grid.tile_height));

        if (gCellId_X >= grid.xGrids && grid.perfect_regular_x)
                gCellId_X--;

        if (gCellId_Y >= grid.yGrids && grid.perfect_regular_y)
                gCellId_Y--;

        DBU centerX = (gCellId_X * grid.tile_width) + (grid.tile_width / 2) + grid.lower_left_x;
        DBU centerY = (gCellId_Y * grid.tile_height) + (grid.tile_height / 2) + grid.lower_left_y;

        pos = DBUxy(centerX, centerY);
}

Bounds FastRouteProcess::globalRoutingToBounds(const FastRoute::ROUTE &route) {
        Rsyn::PhysicalDie phDie = phDesign.getPhysicalDie();
        Bounds dieBounds = phDie.getBounds();
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

        DBU llX = initX - (grid.tile_width / 2);
        DBU llY = initY - (grid.tile_height / 2);

        DBU urX = finalX + (grid.tile_width / 2);
        DBU urY = finalY + (grid.tile_height / 2);

        if (urX > dieBounds.getUpper().x) {
                urX = dieBounds.getUpper().x;
        }
        if (urY > dieBounds.getUpper().y) {
                urY = dieBounds.getUpper().y;
        }

        DBUxy lowerLeft = DBUxy(llX, llY);
        DBUxy upperRight = DBUxy(urX, urY);

        Bounds routeBds = Bounds(lowerLeft, upperRight);
        return routeBds;
}

std::pair<FastRouteProcess::TILE, FastRouteProcess::TILE> FastRouteProcess::getBlockedTiles(const Bounds &obs, Bounds &firstTileBds, Bounds &lastTileBds) {
        std::pair<TILE, TILE> tiles;
        FastRouteProcess::TILE firstTile;
        FastRouteProcess::TILE lastTile;

        DBUxy lower = obs.getLower();  // lower bound of obstacle
        DBUxy upper = obs.getUpper();  // upper bound of obstacle

        getPosOnGrid(lower);  // translate lower bound of obstacle to the center of the tile where it is inside
        getPosOnGrid(upper);  // translate upper bound of obstacle to the center of the tile where it is inside

        // Get x and y indices of first blocked tile
        firstTile.x = (lower.x - (grid.tile_width / 2)) / grid.tile_width;
        firstTile.y = (lower.y - (grid.tile_height / 2)) / grid.tile_height;

        // Get x and y indices of last blocked tile
        lastTile.x = (upper.x - (grid.tile_width / 2)) / grid.tile_width;
        lastTile.y = (upper.y - (grid.tile_height / 2)) / grid.tile_height;

        tiles = std::make_pair(firstTile, lastTile);

        DBUxy llFirstTile = DBUxy(lower.x - (grid.tile_width / 2), lower.y - (grid.tile_height / 2));
        DBUxy urFirstTile = DBUxy(lower.x + (grid.tile_width / 2), lower.y + (grid.tile_height / 2));

        DBUxy llLastTile = DBUxy(upper.x - (grid.tile_width / 2), upper.y - (grid.tile_height / 2));
        DBUxy urLastTile = DBUxy(upper.x + (grid.tile_width / 2), upper.y + (grid.tile_height / 2));

        firstTileBds = Bounds(llFirstTile, urFirstTile);
        lastTileBds = Bounds(llLastTile, urLastTile);

        return tiles;
}

// WORKAROUND: FastRoute doesn't understand pins in other layers than metal1
// This function inserts GCELLs over these pins, to generate a valid guide
void FastRouteProcess::addRemainingGuides(std::vector<FastRoute::NET> &globalRoute) {
        std::map<std::string, std::vector<FastRoute::PIN>> allNets;
        allNets = fastRoute.getNets();
        int localNetsId = allNets.size();

        for (FastRoute::NET &netRoute : globalRoute) {
                std::vector<FastRoute::PIN> &pins = allNets[netRoute.name];

                if (netsDegree[netRoute.name] < 2) {
                        continue;
                }

                int lastLayer = -1;
                for (int p = 0; p < pins.size() - 1; p++)
                        if (pins[p].x == pins[p + 1].x && pins[p].y == pins[p + 1].y)
                                if (pins[p].layer > lastLayer)
                                        lastLayer = pins[p].layer;

                if (netRoute.route.size() == 0) {
                        for (int l = 1; l <= lastLayer - 1; l++) {
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
                                        for (int l = 1; l <= pin.layer - 1; l++) {
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

int FastRouteProcess::computeTileReduce(const Bounds &obs, const Bounds &tile, DBU trackSpace, bool first, bool horizontal) {
        int reduce = -1;
        if (!horizontal) {
                if (first) {
                        reduce = floor(abs(tile.getUpper().x - obs.getLower().x) / trackSpace);
                } else {
                        reduce = floor(abs(obs.getUpper().x - tile.getLower().x) / trackSpace);
                }
        } else {
                if (first) {
                        reduce = floor(abs(tile.getUpper().y - obs.getLower().y) / trackSpace);
                } else {
                        reduce = floor(abs(obs.getUpper().y - tile.getLower().y) / trackSpace);
                }
        }

        if (reduce < 0) {
                std::cout << "Error!!!\n";
                std::exit(0);
        }
        return reduce;
}

void FastRouteProcess::getSpecialNetsObstacles(std::map<int, std::vector<Bounds>> &mapLayerObstacles) {
        for (Rsyn::Net net : module.allNets()) {
                if (net.getUse() == Rsyn::POWER || net.getUse() == Rsyn::GROUND) {
                        Rsyn::PhysicalRouting phRouting = phDesign.getNetRouting(net);

                        for (Rsyn::PhysicalRoutingWire wire : phRouting.allWires()) {  // adding special nets wires
                                int layer = wire.getLayer().getRelativeIndex();

                                DBU xmin = wire.getExtendedSourcePosition().x < wire.getExtendedTargetPosition().x ? wire.getExtendedSourcePosition().x : wire.getExtendedTargetPosition().x;
                                DBU ymin = wire.getExtendedSourcePosition().y < wire.getExtendedTargetPosition().y ? wire.getExtendedSourcePosition().y : wire.getExtendedTargetPosition().y;
                                DBU xmax = wire.getExtendedSourcePosition().x > wire.getExtendedTargetPosition().x ? wire.getExtendedSourcePosition().x : wire.getExtendedTargetPosition().x;
                                DBU ymax = wire.getExtendedSourcePosition().y > wire.getExtendedTargetPosition().y ? wire.getExtendedSourcePosition().y : wire.getExtendedTargetPosition().y;

                                xmin = (wire.getExtendedSourcePosition().x == wire.getExtendedTargetPosition().x) ? xmin - (wire.getWidth() / 2) : xmin;
                                ymin = (wire.getExtendedSourcePosition().y == wire.getExtendedTargetPosition().y) ? ymin - (wire.getWidth() / 2) : ymin;
                                xmax = (wire.getExtendedSourcePosition().x == wire.getExtendedTargetPosition().x) ? xmax + (wire.getWidth() / 2) : xmax;
                                ymax = (wire.getExtendedSourcePosition().y == wire.getExtendedTargetPosition().y) ? ymax + (wire.getWidth() / 2) : ymax;

                                Bounds bds(xmin, ymin, xmax, ymax);

                                mapLayerObstacles[layer].push_back(bds);
                        }

                        for (Rsyn::PhysicalRoutingVia phRoutingVia : phRouting.allVias()) {  // adding special nets vias
                                for (Rsyn::PhysicalViaGeometry viaGeo : phRoutingVia.getVia().allBottomGeometries()) {
                                        int layer = phRoutingVia.getBottomLayer().getRelativeIndex();

                                        DBUxy lower = viaGeo.getBounds().getLower() + phRoutingVia.getPosition();
                                        DBUxy upper = viaGeo.getBounds().getUpper() + phRoutingVia.getPosition();
                                        Bounds bds(lower, upper);
                                        mapLayerObstacles[layer].push_back(bds);
                                }  // end for
                                for (Rsyn::PhysicalViaGeometry viaGeo : phRoutingVia.getVia().allTopGeometries()) {
                                        int layer = phRoutingVia.getTopLayer().getRelativeIndex();

                                        DBUxy lower = viaGeo.getBounds().getLower() + phRoutingVia.getPosition();
                                        DBUxy upper = viaGeo.getBounds().getUpper() + phRoutingVia.getPosition();
                                        Bounds bds(lower, upper);
                                        mapLayerObstacles[layer].push_back(bds);
                                }  // end for
                        }          // end for
                }
        }
}

}  // namespace Rsyn
