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

namespace FastRoute {

void DBWrapper::initGrid(int maxLayer) {
        // WORKAROUND: Initializing _chip here while we don't have a "populateFastRoute" function"
        _chip = _db->getChip();
    
        odb::dbTech* tech = _db->getTech();
        if (!tech) {
                std::cout << "[ERROR] obd::dbTech not initialized! Exiting...\n";
                std::exit(1);
        }

        odb::dbBlock* block = _chip->getBlock();
        if (!block) {
                std::cout << "[ERROR] odb::dbBlock not found! Exiting...\n";
                std::exit(1);
        }
        
        odb::dbTechLayer* selectedLayer = tech->findRoutingLayer(selectedMetal);
        
        if (!selectedLayer) {
                std::cout << "[ERROR] Layer " << selectedMetal << " not found! Exiting...\n";
                std::exit(1);
        }
        
        odb::dbTrackGrid* selectedTrack = block->findTrackGrid(selectedLayer);
        
        if (!selectedTrack) {
                std::cout << "[ERROR] Track for layer " << selectedMetal << " not found! Exiting...\n";
                std::exit(1);
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
                std::cout << "[ERROR] Layer " << selectedMetal << " does not have valid direction! Exiting...\n";
                std::exit(1);
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
                std::cout << "[ERROR] obd::dbTech not initialized! Exiting...\n";
                std::exit(1);
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
                        std::cout << "[ERROR] Layer 1 does not have valid direction! Exiting...\n";
                        std::exit(1);
                }
                
                RoutingLayer routingLayer = RoutingLayer(index, name, preferredDirection);
                routingLayers.push_back(routingLayer);
        }
}

void DBWrapper::initRoutingTracks(std::vector<RoutingTracks>& allRoutingTracks, int maxLayer, std::map<int, float> layerPitches) {
        odb::dbTech* tech = _db->getTech();
        if (!tech) {
                std::cout << "[ERROR] obd::dbTech not initialized! Exiting...\n";
                std::exit(1);
        }

        odb::dbBlock* block = _chip->getBlock();
        if (!block) {
                std::cout << "[ERROR] odb::dbBlock not found! Exiting...\n";
                std::exit(1);
        }
        
        for (int layer = 1; layer <= tech->getRoutingLayerCount(); layer++) {
                if (layer > maxLayer && maxLayer > -1) {
                        break;
                }
            
                odb::dbTechLayer* techayer = tech->findRoutingLayer(layer);
        
                if (!techayer) {
                        std::cout << "[ERROR] Layer" << selectedMetal << " not found! Exiting...\n";
                        std::exit(1);
                }

                odb::dbTrackGrid* selectedTrack = block->findTrackGrid(techayer);

                if (!selectedTrack) {
                        std::cout << "[ERROR] Track for layer " << selectedMetal << " not found! Exiting...\n";
                        std::exit(1);
                }
                
                int trackStepX, trackStepY;
                int initTrackX, numTracksX;
                int initTrackY, numTracksY;
                int spacing, location, numTracks;
                bool orientation;

                selectedTrack->getGridPatternX(0, initTrackX, numTracksX, trackStepX);
                selectedTrack->getGridPatternY(0, initTrackY, numTracksY, trackStepY);

                if (techayer->getDirection().getValue() == odb::dbTechLayerDir::HORIZONTAL) {
                        spacing = trackStepY;
                        if (layerPitches.find(layer) != layerPitches.end()) {
                                int layerPitch = (int)(tech->getLefUnits()*layerPitches[layer]);
                                spacing = std::max(layerPitch, trackStepY);
                        }
                        location = initTrackY;
                        numTracks = numTracksY;
                        orientation = RoutingLayer::HORIZONTAL;
                } else if (techayer->getDirection().getValue() == odb::dbTechLayerDir::VERTICAL) {
                        spacing = trackStepX;
                        if (layerPitches.find(layer) != layerPitches.end()) {
                                int layerPitch = (int)(tech->getLefUnits()*layerPitches[layer]);
                                spacing = std::max(layerPitch, trackStepX);
                        }
                        location = initTrackX;
                        numTracks = numTracksX;
                        orientation = RoutingLayer::VERTICAL;
                } else {
                        std::cout << "[ERROR] Layer " << selectedMetal << " does not have valid direction! Exiting...\n";
                        std::exit(1);
                }
                
                RoutingTracks routingTracks = RoutingTracks(layer, spacing,
                                                           location, numTracks,
                                                           orientation);
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
                std::cout << "[ERROR] obd::dbTech not initialized! Exiting...\n";
                std::exit(1);
        }
        
        odb::dbBlock* block = _chip->getBlock();
        if (!block) {
                std::cout << "[ERROR] odb::dbBlock not found! Exiting...\n";
                std::exit(1);
        }
        
        for (int l = 1; l <= tech->getRoutingLayerCount(); l++) {
                if (l > maxLayer && maxLayer > -1) {
                        break;
                }
                
                odb::dbTechLayer* techLayer = tech->findRoutingLayer(l);
                
                odb::dbTrackGrid* track = block->findTrackGrid(techLayer);
                
                if (!track) {
                        std::cout << "[ERROR] Track for layer " << l << " not found! Exiting...\n";
                        std::exit(1);
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
                        std::cout << "[ERROR] Layer " << l << " does not have valid direction! Exiting...\n";
                        std::exit(1);
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
                std::cout << "[ERROR] obd::dbTech not initialized! Exiting...\n";
                std::exit(1);
        }
        
        odb::dbBlock* block = _chip->getBlock();
        if (!block) {
                std::cout << "[ERROR] odb::dbBlock not found! Exiting...\n";
                std::exit(1);
        }
        
        for (int l = 1; l <= tech->getRoutingLayerCount(); l++) {
                if (l > maxLayer && maxLayer > -1) {
                        break;
                }
            
                odb::dbTechLayer* techLayer = tech->findRoutingLayer(l);
                
                odb::dbTrackGrid* track = block->findTrackGrid(techLayer);
                
                if (!track) {
                        std::cout << "[ERROR] Track for layer " << l << " not found! Exiting...\n";
                        std::exit(1);
                }
                
                track->getGridPatternX(0, initTrackX, numTracksX, trackStepX);
                track->getGridPatternY(0, initTrackY, numTracksY, trackStepY);
                
                if (techLayer->getDirection().getValue() == odb::dbTechLayerDir::HORIZONTAL) {
                        minWidth = trackStepY;
                } else if (techLayer->getDirection().getValue() == odb::dbTechLayerDir::VERTICAL) {
                        minWidth = trackStepX;
                } else {
                        std::cout << "[ERROR] Layer " << l << " does not have valid direction! Exiting...\n";
                        std::exit(1);
                }
                
                _grid->addSpacing(minSpacing, l-1);
                _grid->addMinWidth(minWidth, l-1);
        }
}

void DBWrapper::initNetlist() {
        Box dieArea(_grid->getLowerLeftX(), _grid->getLowerLeftY(),
                    _grid->getUpperRightX(), _grid->getUpperRightY(), -1);
        
        odb::dbBlock* block = _chip->getBlock();
        if (!block) {
                std::cout << "[ERROR] ads::dbBlock not found! Exiting...\n";
                std::exit(1);
        }
        
        odb::dbSet<odb::dbNet> nets = block->getNets();
        
        if (nets.size() == 0) {
                std::cout << "[ERROR] Design without nets. Exiting...\n";
                std::exit(1);
        }
        
        for (odb::dbNet* currNet : nets) {
                std::vector<Pin> netPins;
                
                if (currNet->getSigType().getValue() == odb::dbSigType::POWER ||
                    currNet->getSigType().getValue() == odb::dbSigType::GROUND ||
                    currNet->isSpecial() || currNet->getSWires().size() > 0) {
                        continue;
                }
                std::string netName = currNet->getConstName();
                std::string signalType = currNet->getSigType().getString();
                
                for (odb::dbITerm* currITerm : currNet->getITerms()) {
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
                        
                        std::string instName = currITerm->getInst()->getConstName();
                        pinName = mTerm->getConstName();
                        pinName = instName + ":" + pinName;
                        
                        odb::dbInst* inst = currITerm->getInst();
                        inst->getOrigin(pX, pY);
                        odb::Point origin = odb::Point(pX, pY);
                        odb::dbTransform transform(inst->getOrient(), origin);
                        
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
                                                std::cout << "[WARNING] Pin " << pinName << " is outside die area\n";
                                        }
                                        pinBoxes[pinLayer].push_back(pinBox);
                                }
                                
                                for (auto& layer_boxes : pinBoxes) {
                                        pinLayers.push_back(layer_boxes.first);
                                }
                                
                                Coordinate pinPos = Coordinate(pX, pY);
                                Pin pin = Pin(pinName, pinPos, pinLayers, pinBoxes, netName, false);
                                netPins.push_back(pin);
                        }
                }
                
                for (odb::dbBTerm* currBTerm : currNet->getBTerms()) {
                        int posX, posY;
                        std::string pinName;
                        
                        currBTerm->getFirstPinLocation(posX, posY);
                        
                        std::vector<int> pinLayers;
                        std::map<int, std::vector<Box>> pinBoxes;
                                                
                        pinName = currBTerm->getConstName();
                        
                        for (odb::dbBPin* currBTermPin : currBTerm->getBPins()) {
                                Coordinate lowerBound;
                                Coordinate upperBound;
                                Box pinBox;
                                int pinLayer;
                                
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
                        }
                        
                        for (auto& layer_boxes : pinBoxes) {
                                pinLayers.push_back(layer_boxes.first);
                        }
                        
                        Coordinate pinPos = Coordinate(posX, posY);
                        Pin pin = Pin(pinName, pinPos, pinLayers, pinBoxes, netName, true);
                        netPins.push_back(pin);
                }
                _netlist->addNet(netName, signalType, netPins);
        }
}

void DBWrapper::initObstacles() {
        Box dieArea(_grid->getLowerLeftX(), _grid->getLowerLeftY(),
                    _grid->getUpperRightX(), _grid->getUpperRightY(), -1);
        
        // Get routing obstructions
        odb::dbTech* tech = _db->getTech();
        if (!tech) {
                std::cout << "[ERROR] obd::dbTech not initialized! Exiting...\n";
                std::exit(1);
        }

        odb::dbBlock* block = _chip->getBlock();
        if (!block) {
                std::cout << "[ERROR] odb::dbBlock not found! Exiting...\n";
                std::exit(1);
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

                if (master->getType().isPad()) {
                        continue;
                }
                
                currInst->getOrigin(pX, pY);
                odb::Point origin = odb::Point(pX, pY);
                
                odb::dbTransform transform(currInst->getOrient(), origin);
                
                if (master->isBlock()) {
                        macrosCnt++;
                }
                
                for (odb::dbBox* currBox : master->getObstructions()) {
                        int layer = currBox->getTechLayer()->getRoutingLevel();
                        
                        odb::Rect rect;
                        currBox->getBox(rect);
                        transform.apply(rect);

                        Coordinate lowerBound = Coordinate(rect.xMin(), rect.yMin());
                        Coordinate upperBound = Coordinate(rect.xMax(), rect.yMax());
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
                std::cout << "[ERROR] Design without nets. Exiting...\n";
                std::exit(1);
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
                std::cout << "[ERROR] obd::dbTech not initialized! Exiting...\n";
                std::exit(1);
        }

        odb::dbBlock* block = _chip->getBlock();
        if (!block) {
                std::cout << "[ERROR] odb::dbBlock not found! Exiting...\n";
                std::exit(1);
        }
        
        for (int layer = 1; layer <= tech->getRoutingLayerCount(); layer++) {          
                odb::dbTechLayer* techayer = tech->findRoutingLayer(layer);
                if (!techayer) {
                        std::cout << "[ERROR] Layer" << selectedMetal << " not found! Exiting...\n";
                        std::exit(1);
                }
                odb::dbTrackGrid* selectedTrack = block->findTrackGrid(techayer);
                if (!selectedTrack) {
                    break;
                }
                maxRoutingLayer = layer;
        }
        
        return maxRoutingLayer;
}

std::set<int> DBWrapper::findTransitionLayers(int maxRoutingLayer) {
        std::set<int> transitionLayers;
        odb::dbTech* tech = _db->getTech();
        odb::dbSet<odb::dbTechVia> vias = tech->getVias();
        
        if (vias.size() == 0) {
                std::cout << "[ERROR] Tech without vias. Exiting...\n";
                std::exit(1);
        }
        
        std::vector<odb::dbTechVia*> defaultVias;

        for (odb::dbTechVia* currVia : vias) {
                odb::dbStringProperty* prop = odb::dbStringProperty::find(currVia, "OR_DEFAULT");
                
                if (prop == NULL) {
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




































