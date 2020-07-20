/////////////////////////////////////////////////////////////////////////////
//
// BSD 3-Clause License
//
// Copyright (c) 2019, University of California, San Diego.
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
//
///////////////////////////////////////////////////////////////////////////////

#include "DBWrapper.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>
#include <string>
#include <utility>
#include <map>
#include <set>

#include "Coordinate.h"
#include "Box.h"
#include "Pin.h"
#include "openroad/Error.hh"

namespace FastRoute {

using ord::error;

void DBWrapper::initGrid(int maxLayer) {
        // WORKAROUND: Initializing _chip here while we don't have a "populateFastRoute" function"
        _chip = _db->getChip();
    
        odb::dbTech* tech = _db->getTech();
        if (!tech) {
                error("obd::dbTech not initialized\n");
        }

        odb::dbBlock* block = _chip->getBlock();
        if (!block) {
                error("odb::dbBlock not found\n");
        }
        
        odb::dbTechLayer* selectedLayer = tech->findRoutingLayer(selectedMetal);
        
        if (!selectedLayer) {
                error("Layer %d not found\n", selectedMetal);
        }
        
        odb::dbTrackGrid* selectedTrack = block->findTrackGrid(selectedLayer);
        
        if (!selectedTrack) {
                error("Track for layer %d not found\n", selectedMetal);
        }
        
        int trackStepX, trackStepY;
        int initTrackX, numTracksX;
        int initTrackY, numTracksY;
        int trackSpacing;
        
        selectedTrack->getGridPatternX(0, initTrackX, numTracksX, trackStepX);
        selectedTrack->getGridPatternY(0, initTrackY, numTracksY, trackStepY);
        
        if (selectedLayer->getDirection().getValue() == odb::dbTechLayerDir::HORIZONTAL) {
                trackSpacing = trackStepY;
        } else if (selectedLayer->getDirection().getValue() == odb::dbTechLayerDir::VERTICAL) {
                trackSpacing = trackStepX;
        } else {
                error("Layer %d does not have valid direction\n", selectedMetal);
        }
        
        odb::Rect rect;
        block->getDieArea(rect);
        
        long lowerLeftX = rect.xMin();
        long lowerLeftY = rect.yMin();
        
        long upperRightX = rect.xMax();
        long upperRightY = rect.yMax();
        
        long tileWidth = _grid->getPitchesInTile() * trackSpacing;
        long tileHeight = _grid->getPitchesInTile() * trackSpacing;
        
        int xGrids = std::floor((float)upperRightX / tileWidth);
        int yGrids = std::floor((float)upperRightY / tileHeight);
        
        bool perfectRegularX = false;
        bool perfectRegularY = false;
        
        int numLayers = tech->getRoutingLayerCount();
        if (maxLayer > -1) {
                numLayers = maxLayer;
        }
        
        if ((xGrids * tileWidth) == upperRightX)
                perfectRegularX = true;
        
        if ((yGrids * tileHeight) == upperRightY)
                perfectRegularY = true;
        
        std::vector<int> genericVector(numLayers);
        std::map<int, std::vector<Box>> genericMap;
        
        *_grid = Grid(lowerLeftX, lowerLeftY, rect.xMax(), rect.yMax(),
                     tileWidth, tileHeight, xGrids, yGrids, perfectRegularX,
                     perfectRegularY, numLayers, genericVector, genericVector,
                     genericVector, genericVector, genericMap, tech->getLefUnits());
}

void DBWrapper::initRoutingLayers(std::vector<RoutingLayer>& routingLayers) {
        odb::dbTech* tech = _db->getTech();
        
        if (!tech) {
                error("obd::dbTech not initialized\n");
        }
        
        for (int l = 1; l <= tech->getRoutingLayerCount(); l++) {
                odb::dbTechLayer* techLayer = tech->findRoutingLayer(l);
                int index = l;
                std::string name = techLayer->getConstName();
                bool preferredDirection;
                if (techLayer->getDirection().getValue() == odb::dbTechLayerDir::HORIZONTAL) {
                        preferredDirection = RoutingLayer::HORIZONTAL;
                } else if (techLayer->getDirection().getValue() == odb::dbTechLayerDir::VERTICAL) {
                        preferredDirection = RoutingLayer::VERTICAL;
                } else {
                        error("Layer %d does not have valid direction\n", l);
                }
                
                RoutingLayer routingLayer = RoutingLayer(index, name, preferredDirection);
                routingLayers.push_back(routingLayer);
        }
}

void DBWrapper::initRoutingTracks(std::vector<RoutingTracks>& allRoutingTracks, int maxLayer, std::map<int, float> layerPitches) {
        odb::dbTech* tech = _db->getTech();
        if (!tech) {
                error("obd::dbTech not initialized\n");
        }

        odb::dbBlock* block = _chip->getBlock();
        if (!block) {
                error("odb::dbBlock not found\n");
        }
        
        for (int layer = 1; layer <= tech->getRoutingLayerCount(); layer++) {
                if (layer > maxLayer && maxLayer > -1) {
                        break;
                }
            
                odb::dbTechLayer* techayer = tech->findRoutingLayer(layer);
        
                if (!techayer) {
                        error("Layer %d not found\n", selectedMetal);
                }

                odb::dbTrackGrid* selectedTrack = block->findTrackGrid(techayer);

                if (!selectedTrack) {
                        error("Track for layer %d not found\n", selectedMetal);
                }
                
                int trackStepX, trackStepY;
                int initTrackX, numTracksX;
                int initTrackY, numTracksY;
                int trackPitch, line2ViaPitch, location, numTracks;
                bool orientation;

                selectedTrack->getGridPatternX(0, initTrackX, numTracksX, trackStepX);
                selectedTrack->getGridPatternY(0, initTrackY, numTracksY, trackStepY);

                if (techayer->getDirection().getValue() == odb::dbTechLayerDir::HORIZONTAL) {
                        trackPitch = trackStepY;
                        if (layerPitches.find(layer) != layerPitches.end()) {
                                line2ViaPitch = (int)(tech->getLefUnits()*layerPitches[layer]);
                        } else {
                                line2ViaPitch = -1;
                        }
                        location = initTrackY;
                        numTracks = numTracksY;
                        orientation = RoutingLayer::HORIZONTAL;
                } else if (techayer->getDirection().getValue() == odb::dbTechLayerDir::VERTICAL) {
                        trackPitch = trackStepX;
                        if (layerPitches.find(layer) != layerPitches.end()) {
                                line2ViaPitch = (int)(tech->getLefUnits()*layerPitches[layer]);
                        } else {
                                line2ViaPitch = -1;
                        }
                        location = initTrackX;
                        numTracks = numTracksX;
                        orientation = RoutingLayer::VERTICAL;
                } else {
                        error("Layer %d does not have valid direction! Exiting...\n", selectedMetal);
                }
                
                RoutingTracks routingTracks = RoutingTracks(layer, trackPitch,
                                                           line2ViaPitch, location,
                                                           numTracks, orientation);
                allRoutingTracks.push_back(routingTracks);
        }
}

void DBWrapper::computeCapacities(int maxLayer, std::map<int, float> layerPitches) {
        int trackSpacing;
        int hCapacity, vCapacity;
        int trackStepX, trackStepY;
        
        int initTrackX, numTracksX;
        int initTrackY, numTracksY;
        
        odb::dbTech* tech = _db->getTech();
        
        if (!tech) {
                error("obd::dbTech not initialized\n");
        }
        
        odb::dbBlock* block = _chip->getBlock();
        if (!block) {
                error("odb::dbBlock not found\n");
        }
        
        for (int l = 1; l <= tech->getRoutingLayerCount(); l++) {
                if (l > maxLayer && maxLayer > -1) {
                        break;
                }
                
                odb::dbTechLayer* techLayer = tech->findRoutingLayer(l);
                
                odb::dbTrackGrid* track = block->findTrackGrid(techLayer);
                
                if (!track) {
                        error("Track for layer %d not found\n", l);
                }
                
                track->getGridPatternX(0, initTrackX, numTracksX, trackStepX);
                track->getGridPatternY(0, initTrackY, numTracksY, trackStepY);
                
                if (techLayer->getDirection().getValue() == odb::dbTechLayerDir::HORIZONTAL) {
                        trackSpacing = trackStepY;

                        if (layerPitches.find(l) != layerPitches.end()) {
                                int layerPitch = (int)(tech->getLefUnits()*layerPitches[l]);
                                trackSpacing = std::max(layerPitch, trackStepY);
                        }

                        hCapacity = std::floor((float)_grid->getTileWidth() / trackSpacing);
                        
                        _grid->addHorizontalCapacity(hCapacity, l-1);
                        _grid->addVerticalCapacity(0, l-1);
                } else if (techLayer->getDirection().getValue() == odb::dbTechLayerDir::VERTICAL) {
                        trackSpacing = trackStepX;

                        if (layerPitches.find(l) != layerPitches.end()) {
                                int layerPitch = (int)(tech->getLefUnits()*layerPitches[l]);
                                trackSpacing = std::max(layerPitch, trackStepX);
                        }

                        vCapacity = std::floor((float)_grid->getTileWidth() / trackSpacing);
                        
                        _grid->addHorizontalCapacity(0, l-1);
                        _grid->addVerticalCapacity(vCapacity, l-1);
                } else {
                        error("Layer %d does not have valid direction\n", l);
                }
        }
}

void DBWrapper::computeSpacingsAndMinWidth(int maxLayer) {
        int minSpacing = 0;
        int minWidth;
        int trackStepX, trackStepY;
        int initTrackX, numTracksX;
        int initTrackY, numTracksY;
        
        odb::dbTech* tech = _db->getTech();
        
        if (!tech) {
                error("obd::dbTech not initialized\n");
        }
        
        odb::dbBlock* block = _chip->getBlock();
        if (!block) {
                error("odb::dbBlock not found\n");
        }
        
        for (int l = 1; l <= tech->getRoutingLayerCount(); l++) {
                if (l > maxLayer && maxLayer > -1) {
                        break;
                }
            
                odb::dbTechLayer* techLayer = tech->findRoutingLayer(l);
                
                odb::dbTrackGrid* track = block->findTrackGrid(techLayer);
                
                if (!track) {
                        error("Track for layer %d not found\n", l);
                }
                
                track->getGridPatternX(0, initTrackX, numTracksX, trackStepX);
                track->getGridPatternY(0, initTrackY, numTracksY, trackStepY);
                
                if (techLayer->getDirection().getValue() == odb::dbTechLayerDir::HORIZONTAL) {
                        minWidth = trackStepY;
                } else if (techLayer->getDirection().getValue() == odb::dbTechLayerDir::VERTICAL) {
                        minWidth = trackStepX;
                } else {
                        error("Layer %d does not have valid direction\n", l);
                }
                
                _grid->addSpacing(minSpacing, l-1);
                _grid->addMinWidth(minWidth, l-1);
        }
}

void DBWrapper::addNet(odb::dbNet* net, Box dieArea, bool isClock) {
        odb::dbTech* tech = _db->getTech();

        std::vector<Pin> netPins;
                
        if (net->getSigType().getValue() == odb::dbSigType::POWER ||
            net->getSigType().getValue() == odb::dbSigType::GROUND ||
            net->isSpecial() || net->getSWires().size() > 0) {
                return;
        }
        std::string netName = net->getConstName();
        std::string signalType = isClock ? "CLOCK" : net->getSigType().getString();
        
        for (odb::dbITerm* currITerm : net->getITerms()) {
                int pX, pY;
                std::string pinName;
                std::vector<int> pinLayers;
                std::map<int, std::vector<Box>> pinBoxes;
                
                odb::dbMTerm* mTerm = currITerm->getMTerm();
                odb::dbMaster* master = mTerm->getMaster();
                
                if (master->getType() == odb::dbMasterType::COVER || 
                    master->getType() == odb::dbMasterType::COVER_BUMP) {
                        std::cout << "[WARNING] Net connected with instance of class COVER added for routing\n";
                }

                bool connectedToPad = master->getType().isPad();
                bool connectedToMacro = master->isBlock();
                
                std::string instName = currITerm->getInst()->getConstName();
                pinName = mTerm->getConstName();
                pinName = instName + "/" + pinName;
                Coordinate pinPos;

                Pin::Type type(Pin::Type::OTHER);
                if (mTerm->getIoType() == odb::dbIoType::INPUT) {
                        type = Pin::SINK;
                } else if (mTerm->getIoType() == odb::dbIoType::OUTPUT) {
                        type = Pin::SOURCE;
                }
                
                odb::dbInst* inst = currITerm->getInst();
                inst->getOrigin(pX, pY);
                odb::Point origin = odb::Point(pX, pY);
                odb::dbTransform transform(inst->getOrient(), origin);

                odb::dbBox* instBox = inst->getBBox();
                Coordinate instMiddle = Coordinate((instBox->xMin() + (instBox->xMax() - instBox->xMin()) / 2.0),
                                                   (instBox->yMin() + (instBox->yMax() - instBox->yMin()) / 2.0));
                
                for (odb::dbMPin* currMTermPin : mTerm->getMPins()) {
                        Coordinate lowerBound;
                        Coordinate upperBound;
                        Box pinBox;
                        int pinLayer;
                        int lastLayer = -1;

                        for (odb::dbBox* box : currMTermPin->getGeometry()) {
                                odb::Rect rect;
                                box->getBox(rect);
                                transform.apply(rect);
                                
                                odb::dbTechLayer* techLayer = box->getTechLayer();
                                if (techLayer->getType().getValue() != odb::dbTechLayerType::ROUTING) {
                                        continue;
                                }
                                
                                pinLayer = techLayer->getRoutingLevel();
                                lowerBound = Coordinate(rect.xMin(), 
                                                        rect.yMin());
                                upperBound = Coordinate(rect.xMax(), 
                                                        rect.yMax());
                                pinBox = Box(lowerBound, upperBound, pinLayer);
                                if (!dieArea.inside(pinBox)) {
                                        std::cout << "[WARNING] Pin " << pinName << " is outside die area\n";
                                }
                                pinBoxes[pinLayer].push_back(pinBox);
                                if (pinLayer > lastLayer) {
                                        pinPos = lowerBound;
                                }
                        }
                }

                for (auto& layer_boxes : pinBoxes) {
                        pinLayers.push_back(layer_boxes.first);
                }

                Pin pin = Pin(pinName, pinPos, pinLayers, Orientation::INVALID, pinBoxes, netName, false, (connectedToPad || connectedToMacro), type);

                if (connectedToPad || connectedToMacro) {
                        Coordinate pinPosition = pin.getPosition();
                        odb::dbTechLayer* techLayer = tech->findRoutingLayer(pin.getTopLayer());

                        if (techLayer->getDirection().getValue() == odb::dbTechLayerDir::HORIZONTAL) {
                                DBU instToPin = pinPosition.getX() - instMiddle.getX();
                                if (instToPin < 0) {
                                        pin.setOrientation(Orientation::ORIENT_EAST);
                                } else {
                                        pin.setOrientation(Orientation::ORIENT_WEST);
                                }
                        } else if (techLayer->getDirection().getValue() == odb::dbTechLayerDir::VERTICAL) {
                                DBU instToPin = pinPosition.getY() - instMiddle.getY();
                                if (instToPin < 0) {
                                        pin.setOrientation(Orientation::ORIENT_NORTH);
                                } else {
                                        pin.setOrientation(Orientation::ORIENT_SOUTH);
                                }
                        }
                }

                netPins.push_back(pin);
        }
        
        for (odb::dbBTerm* currBTerm : net->getBTerms()) {
                int posX, posY;
                std::string pinName;
                
                currBTerm->getFirstPinLocation(posX, posY);
                odb::dbITerm* iTerm = currBTerm->getITerm();
                odb::dbMTerm* mTerm;
                odb::dbMaster* master;
                bool connectedToPad = false;
                bool connectedToMacro = false;
                odb::dbInst* inst;
                odb::dbBox* instBox;
                Coordinate instMiddle = Coordinate(-1, -1);

                if (iTerm != nullptr) {
                        mTerm = iTerm->getMTerm();
                        master = mTerm->getMaster();
                        connectedToPad = master->getType().isPad();
                        connectedToMacro = master->isBlock();

                        inst = iTerm->getInst();
                        instBox = inst->getBBox();
                        instMiddle = Coordinate((instBox->xMin() + (instBox->xMax() - instBox->xMin()) / 2.0),
                                                (instBox->yMin() + (instBox->yMax() - instBox->yMin()) / 2.0));
                }
                
                std::vector<int> pinLayers;
                std::map<int, std::vector<Box>> pinBoxes;
                                        
                pinName = currBTerm->getConstName();
                Coordinate pinPos;

                Pin::Type type(Pin::Type::OTHER);
                if (currBTerm->getIoType() == odb::dbIoType::INPUT) {
                        type = Pin::SOURCE;
                } else if (currBTerm->getIoType() == odb::dbIoType::OUTPUT) {
                        type = Pin::SINK;
                }

                for (odb::dbBPin* currBTermPin : currBTerm->getBPins()) {
                        Coordinate lowerBound;
                        Coordinate upperBound;
                        Box pinBox;
                        int pinLayer;
                        int lastLayer = -1;
                        
                        odb::dbBox* currBTermBox = currBTermPin->getBox();
                        odb::dbTechLayer* techLayer = currBTermBox->getTechLayer();
                        if (techLayer->getType().getValue() != odb::dbTechLayerType::ROUTING) {
                                continue;
                        }
                        
                        pinLayer = techLayer->getRoutingLevel();
                        lowerBound = Coordinate(currBTermBox->xMin(), 
                                                currBTermBox->yMin());
                        upperBound = Coordinate(currBTermBox->xMax(), 
                                                currBTermBox->yMax());
                        pinBox = Box(lowerBound, upperBound, pinLayer);
                        if (!dieArea.inside(pinBox)) {
                                std::cout << "[WARNING] Pin " << pinName << " is outside die area\n";
                        }
                        pinBoxes[pinLayer].push_back(pinBox);

                        if (pinLayer > lastLayer) {
                                pinPos = lowerBound;
                        }
                }
                
                for (auto& layer_boxes : pinBoxes) {
                        pinLayers.push_back(layer_boxes.first);
                }
                
                Pin pin = Pin(pinName, pinPos, pinLayers, Orientation::INVALID, pinBoxes, netName, true, (connectedToPad || connectedToMacro), type);

                if (connectedToPad) {
                        Coordinate pinPosition = pin.getPosition();
                        odb::dbTechLayer* techLayer = tech->findRoutingLayer(pin.getTopLayer());
                        
                        if (techLayer->getDirection().getValue() == odb::dbTechLayerDir::HORIZONTAL) {
                                DBU instToPin = pinPosition.getX() - instMiddle.getX();
                                if (instToPin < 0) {
                                        pin.setOrientation(Orientation::ORIENT_EAST);
                                } else {
                                        pin.setOrientation(Orientation::ORIENT_WEST);
                                }
                        } else if (techLayer->getDirection().getValue() == odb::dbTechLayerDir::VERTICAL) {
                                DBU instToPin = pinPosition.getY() - instMiddle.getY();
                                if (instToPin < 0) {
                                        pin.setOrientation(Orientation::ORIENT_NORTH);
                                } else {
                                        pin.setOrientation(Orientation::ORIENT_SOUTH);
                                }
                        }
                }

                netPins.push_back(pin);
        }
        _netlist->addNet(netName, signalType, netPins);
}

void DBWrapper::initNetlist() {
        Box dieArea(_grid->getLowerLeftX(), _grid->getLowerLeftY(),
                    _grid->getUpperRightX(), _grid->getUpperRightY(), -1);
        
        odb::dbBlock* block = _chip->getBlock();
        if (!block) {
                error("odb::dbBlock not found\n");
        }
        
        std::vector<odb::dbNet*> nets;

        for (odb::dbNet* net : block->getNets()) {
                if (_clockNets.find(net) == _clockNets.end()) {
                        nets.push_back(net);
                }
        }
        
        if (nets.size() == 0 && _clockNets.size() == 0) {
                error("Design without nets");
        }
        
        for (odb::dbNet* net : nets) {
                addNet(net, dieArea, false);
        }
}

void DBWrapper::initClockNets() {
        Box dieArea(_grid->getLowerLeftX(), _grid->getLowerLeftY(),
                    _grid->getUpperRightX(), _grid->getUpperRightY(), -1);

        ord::OpenRoad* openRoad = ord::OpenRoad::openRoad();
        _openSta = openRoad->getSta();

        _openSta->findClkNets(_clockNets);

        std::cout << "[INFO] Found " << _clockNets.size() << " clock nets\n";

        for (odb::dbNet* net : _clockNets) {
                addNet(net, dieArea, true);
        }
}

void DBWrapper::initObstacles() {
        Box dieArea(_grid->getLowerLeftX(), _grid->getLowerLeftY(),
                    _grid->getUpperRightX(), _grid->getUpperRightY(), -1);
        
        // Get routing obstructions
        odb::dbTech* tech = _db->getTech();
        if (!tech) {
                error("obd::dbTech not initialized");
        }

        odb::dbBlock* block = _chip->getBlock();
        if (!block) {
                error("odb::dbBlock not found\n");
        }

        std::map<std::string, uint> layerExtensions;

        for (odb::dbTechLayer* obstructLayer : tech->getLayers()) {

                if (obstructLayer->getType().getValue() != odb::dbTechLayerType::ROUTING){
                        continue;
                }

                int maxInt = std::numeric_limits<int>::max();

                //Gets the smallest possible minimum spacing that won't cause violations for ANY configuration of PARALLELRUNLENGTH (the biggest value in the table)
                
                uint macroExtension = obstructLayer->getSpacing(maxInt,maxInt);

                odb::dbSet<odb::dbTechLayerSpacingRule> eolRules;

                //Check for EOL spacing values and, if the spacing is higher than the one found, use them as the macro extension instead of PARALLELRUNLENGTH

                if (obstructLayer->getV54SpacingRules(eolRules)){
                        for (odb::dbTechLayerSpacingRule* currentRule : eolRules){
                                uint currentSpacing = currentRule->getSpacing();
                                if (currentSpacing > macroExtension){
                                        macroExtension = currentSpacing;
                                }
                        }
                }

                //Check for TWOWIDTHS table values and, if the spacing is higher than the one found, use them as the macro extension instead of PARALLELRUNLENGTH

                if(obstructLayer->hasTwoWidthsSpacingRules()){
                        std::vector<std::vector<uint>> spacingTable;
                        obstructLayer->getTwoWidthsSpacingTable(spacingTable);
                        if (!spacingTable.empty()){
                                std::vector<uint> lastRow = spacingTable.back();
                                uint lastValue = lastRow.back();
                                if (lastValue > macroExtension){
                                        macroExtension = lastValue;
                                }
                        }
                }
                
                //Save the extension to use when defining Macros

                layerExtensions[obstructLayer->getName()] = macroExtension;
        }

        int obstructionsCnt = 0;

        for (odb::dbObstruction* currObstruct : block->getObstructions()) {
                odb::dbBox* obstructBox = currObstruct->getBBox();
                
                int layer = obstructBox->getTechLayer()->getRoutingLevel();
                
                Coordinate lowerBound = Coordinate(obstructBox->xMin(), obstructBox->yMin());
                Coordinate upperBound = Coordinate(obstructBox->xMax(), obstructBox->yMax());
                Box obstacleBox = Box(lowerBound, upperBound, layer);
                if (!dieArea.inside(obstacleBox)) {
                        std::cout << "[WARNING] Found obstacle outside die area\n";
                }
                _grid->addObstacle(layer, obstacleBox);
                obstructionsCnt++;
        }
        
        std::cout << "[INFO] #DB Obstructions: " << obstructionsCnt << "\n";

        // Get instance obstructions
        int macrosCnt = 0;
        int obstaclesCnt = 0;
        for (odb::dbInst* currInst : block->getInsts()) {
                int pX, pY;

                odb::dbMaster* master = currInst->getMaster();
                
                currInst->getOrigin(pX, pY);
                odb::Point origin = odb::Point(pX, pY);
                
                odb::dbTransform transform(currInst->getOrient(), origin);

                bool isMacro = false;
                if (master->isBlock()) {
                        macrosCnt++;
                        isMacro = true;
                }
                
                for (odb::dbBox* currBox : master->getObstructions()) {
                        int layer = currBox->getTechLayer()->getRoutingLevel();
                        
                        odb::Rect rect;
                        currBox->getBox(rect);
                        transform.apply(rect);

                        uint macroExtension = 0;

                        if (isMacro){
                                macroExtension = layerExtensions[currBox->getTechLayer()->getName()];
                        }

                        Coordinate lowerBound = Coordinate(rect.xMin() - macroExtension, rect.yMin() - macroExtension);
                        Coordinate upperBound = Coordinate(rect.xMax() + macroExtension, rect.yMax() + macroExtension);
                        Box obstacleBox = Box(lowerBound, upperBound, layer);
                        if (!dieArea.inside(obstacleBox)) {
                                std::cout << "[WARNING] Found obstacle outside die area in instance " << currInst->getConstName() << "\n";
                        }
                        _grid->addObstacle(layer, obstacleBox);
                        obstaclesCnt++;
                }

                for (odb::dbMTerm* mTerm : master->getMTerms()) {
                        for (odb::dbMPin* currMTermPin : mTerm->getMPins()) {
                                Coordinate lowerBound;
                                Coordinate upperBound;
                                Box pinBox;
                                int pinLayer;

                                for (odb::dbBox* box : currMTermPin->getGeometry()) {
                                        odb::Rect rect;
                                        box->getBox(rect);
                                        transform.apply(rect);

                                        odb::dbTechLayer* techLayer = box->getTechLayer();
                                        if (techLayer->getType().getValue() != odb::dbTechLayerType::ROUTING) {
                                                continue;
                                        }

                                        pinLayer = techLayer->getRoutingLevel();
                                        lowerBound = Coordinate(rect.xMin(), 
                                                                rect.yMin());
                                        upperBound = Coordinate(rect.xMax(), 
                                                                rect.yMax());
                                        pinBox = Box(lowerBound, upperBound, pinLayer);
                                        if (!dieArea.inside(pinBox)) {
                                                std::cout << "[WARNING] Found pin outside die area in instance " << currInst->getConstName() << "\n";
                                        }
                                        _grid->addObstacle(pinLayer, pinBox);
                                }
                        }
                }
        }

        std::cout << "[INFO] #DB Obstacles: " << obstaclesCnt << "\n";
        std::cout << "[INFO] #DB Macros: " << macrosCnt << "\n";
        
        // Get nets obstructions (routing wires and pdn wires)
        odb::dbSet<odb::dbNet> nets = block->getNets();
        
        if (nets.size() == 0) {
                error("Design without nets\n");
        }
        
        for (odb::dbNet* currNet : nets) {
                uint wireCnt = 0, viaCnt = 0;
                currNet->getWireCount(wireCnt, viaCnt);
                if (wireCnt < 1)
                        continue;
                
                if (currNet->getSigType() == odb::dbSigType::POWER ||
                    currNet->getSigType() == odb::dbSigType::GROUND) {
                        for (odb::dbSWire* swire : currNet->getSWires()) {
                                for (odb::dbSBox* s : swire->getWires()) {
                                        if (s->isVia()) {
                                                continue;
                                        } else {
                                                odb::Rect wireRect;
                                                s->getBox(wireRect);
                                                int l = s->getTechLayer()->getRoutingLevel();
                                                
                                                Coordinate lowerBound = Coordinate(wireRect.xMin(), wireRect.yMin());
                                                Coordinate upperBound = Coordinate(wireRect.xMax(), wireRect.yMax());
                                                Box obstacleBox = Box(lowerBound, upperBound, l);
                                                if (!dieArea.inside(obstacleBox)) {
                                                        std::cout << "[WARNING] Net " << currNet->getConstName()
                                                                  << " has wires outside die area\n";
                                                }
                                                _grid->addObstacle(l, obstacleBox);
                                        }
                                }
                        }
                } else {
                        odb::dbWirePath path;
                        odb::dbWirePathShape pshape;
                        odb::dbWire* wire = currNet->getWire();
                        
                        odb::dbWirePathItr pitr;
                        for (pitr.begin(wire); pitr.getNextPath(path);) {
                                while (pitr.getNextShape(pshape)) {
                                        if (pshape.shape.isVia()) {
                                                continue;
                                        } else {
                                                odb::Rect wireRect;
                                                pshape.shape.getBox(wireRect);
                                                int l = pshape.shape.getTechLayer()->getRoutingLevel();
                                                
                                                Coordinate lowerBound = Coordinate(wireRect.xMin(), wireRect.yMin());
                                                Coordinate upperBound = Coordinate(wireRect.xMax(), wireRect.yMax());
                                                Box obstacleBox = Box(lowerBound, upperBound, l);
                                                if (!dieArea.inside(obstacleBox)) {
                                                        std::cout << "[WARNING] Net " << currNet->getConstName()
                                                                  << " has wires outside die area\n";
                                                }
                                                _grid->addObstacle(l, obstacleBox);
                                        }
                                }
                        }
                }
        }
}

int DBWrapper::computeMaxRoutingLayer() {
        _chip = _db->getChip();
        
        int maxRoutingLayer = -1;

        odb::dbTech* tech = _db->getTech();
        if (!tech) {
                error("obd::dbTech not initialized\n");
        }

        odb::dbBlock* block = _chip->getBlock();
        if (!block) {
                error("odb::dbBlock not found\n");
        }
        
        for (int layer = 1; layer <= tech->getRoutingLayerCount(); layer++) {          
                odb::dbTechLayer* techLayer = tech->findRoutingLayer(layer);
                if (!techLayer) {
                        std::cout << "[ERROR] Layer" << selectedMetal << " not found! Exiting...\n";
                        std::exit(1);
                }
                odb::dbTrackGrid* selectedTrack = block->findTrackGrid(techLayer);
                if (!selectedTrack) {
                    break;
                }
                maxRoutingLayer = layer;
        }
        
        return maxRoutingLayer;
}

void DBWrapper::getCutLayerRes(unsigned belowLayerId, float& r) {
        odb::dbBlock* block = _chip->getBlock();
        odb::dbTech* tech = _db->getTech();
        odb::dbTechLayer* cut = tech->findRoutingLayer(belowLayerId)->getUpperLayer();
        r = cut->getResistance(); // assumes single cut
}

void DBWrapper::getLayerRC(unsigned layerId, float& r, float& c) {
        odb::dbBlock* block = _chip->getBlock();
        odb::dbTech* tech = _db->getTech();
        odb::dbTechLayer* techLayer = tech->findRoutingLayer(layerId);

        float layerWidth = (float) techLayer->getWidth() /
                            block->getDbUnitsPerMicron();
        float resOhmPerMicron =  techLayer->getResistance() / layerWidth;
        float capPfPerMicron = layerWidth * techLayer->getCapacitance()
                               + 2 * techLayer->getEdgeCapacitance();

        r = 1E+6 * resOhmPerMicron; // Meters
        c = 1E+6 * 1E-12 * capPfPerMicron; // F/m
}

float DBWrapper::dbuToMeters(unsigned dbu) {
        odb::dbBlock* block = _chip->getBlock();
        return (float) dbu / (block->getDbUnitsPerMicron() * 1E+6);
}

std::set<int> DBWrapper::findTransitionLayers(int maxRoutingLayer) {
        std::set<int> transitionLayers;
        odb::dbTech* tech = _db->getTech();
        odb::dbSet<odb::dbTechVia> vias = tech->getVias();
        
        if (vias.size() == 0) {
                error("Tech without vias\n");
        }
        
        std::vector<odb::dbTechVia*> defaultVias;

        for (odb::dbTechVia* currVia : vias) {
                odb::dbStringProperty* prop = odb::dbStringProperty::find(currVia, "OR_DEFAULT");
                
                if (prop == nullptr) {
                        continue;
                } else {
                        std::cout << "[INFO] Default via: " << currVia->getConstName() << "\n";
                        defaultVias.push_back(currVia);
                }
        }

        if (defaultVias.size() == 0) {
                std::cout << "[WARNING]No OR_DEFAULT vias defined\n";
                for (odb::dbTechVia* currVia : vias) {
                        defaultVias.push_back(currVia);
                }
        }

        for (odb::dbTechVia* currVia : defaultVias) {
                int bottomSize = -1;
                int tmpLen;

                odb::dbTechLayer *bottomLayer;
                odb::dbSet<odb::dbBox> viaBoxes = currVia->getBoxes();
                odb::dbSet<odb::dbBox>::iterator boxIter;

                for (boxIter = viaBoxes.begin(); boxIter != viaBoxes.end(); boxIter++) {
                        odb::dbBox* currBox = *boxIter;
                        odb::dbTechLayer* layer = currBox->getTechLayer();

                        if (layer->getDirection().getValue() == odb::dbTechLayerDir::HORIZONTAL) {
                                tmpLen = currBox->yMax() - currBox->yMin();
                        } else if (layer->getDirection().getValue() == odb::dbTechLayerDir::VERTICAL) {
                                tmpLen = currBox->xMax() - currBox->xMin();
                        } else {
                              continue;  
                        }

                        if (layer->getConstName() == currVia->getBottomLayer()->getConstName()) {
                                bottomLayer = layer;
                                if (tmpLen >= bottomSize) {
                                        bottomSize = tmpLen;
                                }
                        }
                }

                if (bottomLayer->getRoutingLevel() >= maxRoutingLayer || bottomLayer->getRoutingLevel() <= 4)
                        continue;

                if (bottomSize > bottomLayer->getWidth()) {
                        transitionLayers.insert(bottomLayer->getRoutingLevel());
                }
        }

        return transitionLayers;
}

}




































