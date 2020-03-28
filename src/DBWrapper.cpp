#include "DBWrapper.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>
#include <string>
#include <utility>
#include <map>

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
        
        long upperRightX = rect.xMax() - lowerLeftX;
        long upperRightY = rect.yMax() - lowerLeftY;
        
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
        
        bool metal1Orientation = 0;
        
        odb::dbTechLayer* layer1 = tech->findRoutingLayer(1);
        
        if (layer1->getDirection().getValue() == odb::dbTechLayerDir::HORIZONTAL) {
                metal1Orientation = RoutingLayer::HORIZONTAL;
        } else if (layer1->getDirection().getValue() == odb::dbTechLayerDir::VERTICAL) {
                metal1Orientation = RoutingLayer::VERTICAL;
        } else {
                std::cout << "[ERROR] Layer 1 does not have valid direction! Exiting...\n";
                std::exit(1);
        }
        
        std::vector<int> genericVector(numLayers);
        std::map<int, std::vector<Box>> genericMap;
        
        *_grid = Grid(lowerLeftX, lowerLeftY, rect.xMax(), rect.yMax(),
                     tileWidth, tileHeight, xGrids, yGrids, perfectRegularX,
                     perfectRegularY, numLayers, genericVector, genericVector,
                     genericVector, genericVector, genericMap);
}

void DBWrapper::initRoutingLayers(std::vector<RoutingLayer>& routingLayers, int maxLayer) {
        odb::dbTech* tech = _db->getTech();
        
        if (!tech) {
                std::cout << "[ERROR] obd::dbTech not initialized! Exiting...\n";
                std::exit(1);
        }
        
        for (int l = 1; l <= tech->getRoutingLayerCount(); l++) {
                if (l > maxLayer && maxLayer > -1) {
                        break;
                }
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

void DBWrapper::initRoutingTracks(std::vector<RoutingTracks>& allRoutingTracks, int maxLayer) {
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
                        location = initTrackY;
                        numTracks = numTracksY;
                        orientation = RoutingLayer::HORIZONTAL;
                } else if (techayer->getDirection().getValue() == odb::dbTechLayerDir::VERTICAL) {
                        spacing = trackStepX;
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

void DBWrapper::computeCapacities(int maxLayer) {
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
                        hCapacity = std::floor((float)_grid->getTileWidth() / trackSpacing);
                        
                        _grid->addHorizontalCapacity(hCapacity, l-1);
                        _grid->addVerticalCapacity(0, l-1);
                } else if (techLayer->getDirection().getValue() == odb::dbTechLayerDir::VERTICAL) {
                        trackSpacing = trackStepX;
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
        
        odb::dbSet<odb::dbNet>::iterator nIter;
        
        for (nIter = nets.begin(); nIter != nets.end(); ++nIter) {
                bool padFound = false;
                std::vector<Pin> netPins;
                
                odb::dbNet* currNet = *nIter;
                if (currNet->getSigType().getValue() == odb::dbSigType::POWER ||
                    currNet->getSigType().getValue() == odb::dbSigType::GROUND ||
                    currNet->getSWires().size() > 0) {
                        continue;
                }
                std::string netName =currNet->getConstName();
                std::string signalType = currNet->getSigType().getString();
                
                // Iterate through all instance pins
                odb::dbSet<odb::dbITerm> iTerms = currNet->getITerms();
                odb::dbSet<odb::dbITerm>::iterator iIter;
                
                for (iIter = iTerms.begin(); iIter != iTerms.end(); iIter++) {
                        odb::dbITerm* currITerm = *iIter;
                        int pX, pY;
                        std::string pinName;
                        std::vector<int> pinLayers;
                        std::map<int, std::vector<Box>> pinBoxes;
                        
                        odb::dbMTerm* mTerm = currITerm->getMTerm();
                        odb::dbMaster* master = mTerm->getMaster();
                        
                        if (master->getType() == odb::dbMasterType::PAD ||
                            master->getType() == odb::dbMasterType::PAD_INPUT ||
                            master->getType() == odb::dbMasterType::PAD_OUTPUT ||
                            master->getType() == odb::dbMasterType::PAD_INOUT ||
                            master->getType() == odb::dbMasterType::PAD_POWER ||
                            master->getType() == odb::dbMasterType::PAD_SPACER) {
                                padFound = true;
                                break;
                        }
                        
                        std::string instName = currITerm->getInst()->getConstName();
                        pinName = mTerm->getConstName();
                        pinName = instName + ":" + pinName;
                        
                        odb::dbSet<odb::dbMPin> mTermPins = mTerm->getMPins();
                        odb::dbSet<odb::dbMPin>::iterator pinIter;
                        
                        odb::dbInst* inst = currITerm->getInst();
                        inst->getOrigin(pX, pY);
                        odb::Point origin = odb::Point(pX, pY);
                        odb::dbTransform transform(inst->getOrient(), origin);
                        
                        for (pinIter = mTermPins.begin(); pinIter != mTermPins.end(); pinIter++) {
                                Coordinate lowerBound;
                                Coordinate upperBound;
                                Box pinBox;
                                int pinLayer;
                                
                                odb::dbMPin* currMTermPin = *pinIter;
                                odb::dbSet<odb::dbBox> geometries = currMTermPin->getGeometry();
                                odb::dbSet<odb::dbBox>::iterator geomIter;
                                
                                for (geomIter = geometries.begin(); geomIter != geometries.end(); geomIter++) {
                                        odb::dbBox* box = *geomIter;
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
                                        pinBoxes[pinLayer].push_back(pinBox);
                                }
                                
                                for(std::map<int, std::vector<Box>>::iterator it = pinBoxes.begin();
                                    it != pinBoxes.end(); ++it) {
                                        pinLayers.push_back(it->first);
                                }
                                
                                Coordinate pinPos = Coordinate(pX, pY);
                                Pin pin = Pin(pinName, pinPos, pinLayers, pinBoxes, netName, false);
                                netPins.push_back(pin);
                        }
                }
                
                if (padFound) {
                        continue;
                }
                
                // Iterate through all I/O pins
                odb::dbSet<odb::dbBTerm> bTerms = currNet->getBTerms();
                odb::dbSet<odb::dbBTerm>::iterator bIter;
                
                for (bIter = bTerms.begin(); bIter != bTerms.end(); bIter++) {
                        int posX, posY;
                        odb::dbBTerm* currBTerm = *bIter;
                        std::string pinName;
                        
                        currBTerm->getFirstPinLocation(posX, posY);
                        
                        std::vector<int> pinLayers;
                        std::map<int, std::vector<Box>> pinBoxes;
                                                
                        pinName = currBTerm->getConstName();
                        odb::dbSet<odb::dbBPin> bTermPins = currBTerm->getBPins();
                        odb::dbSet<odb::dbBPin>::iterator pinIter;
                        
                        for (pinIter = bTermPins.begin(); pinIter != bTermPins.end(); pinIter++) {
                                Coordinate lowerBound;
                                Coordinate upperBound;
                                Box pinBox;
                                int pinLayer;
                                
                                odb::dbBPin* currBTermPin = *pinIter;
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
                                pinBoxes[pinLayer].push_back(pinBox);
                        }
                        
                        for(std::map<int, std::vector<Box>>::iterator it = pinBoxes.begin();
                            it != pinBoxes.end(); ++it) {
                                pinLayers.push_back(it->first);
                        }
                        
                        Coordinate pinPos = Coordinate(posX, posY);
                        Pin pin = Pin(pinName, pinPos, pinLayers, pinBoxes, netName, true);
                        netPins.push_back(pin);
                }
                _netlist->addNet(netName, signalType, netPins);
        }
}

void DBWrapper::initObstacles() {
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

        odb::dbSet<odb::dbObstruction> obstructions = block->getObstructions();
        
        odb::dbSet<odb::dbObstruction>::iterator obstructIter;
        for (obstructIter = obstructions.begin(); obstructIter != obstructions.end(); obstructIter++) {
                odb::dbObstruction* currObstruct = *obstructIter;
                odb::dbBox* obstructBox = currObstruct->getBBox();
                
                int layer = obstructBox->getTechLayer()->getRoutingLevel();
                
                Coordinate lowerBound = Coordinate(obstructBox->xMin(), obstructBox->yMin());
                Coordinate upperBound = Coordinate(obstructBox->xMax(), obstructBox->yMax());
                Box obstacleBox = Box(lowerBound, upperBound, layer);
                _grid->addObstacle(layer, obstacleBox);
        }
        
        // Get instance obstructions
        odb::dbSet<odb::dbInst> insts;
        insts = block->getInsts();
        
        odb::dbSet<odb::dbInst>::iterator instIter;
        
        for (instIter = insts.begin(); instIter != insts.end(); instIter++) {
                int pX, pY;
                odb::dbInst* currInst = *instIter;
                odb::dbMaster* master = currInst->getMaster();
                
                currInst->getOrigin(pX, pY);
                odb::Point origin = odb::Point(pX, pY);
                
                odb::dbTransform transform(currInst->getOrient(), origin);
                
                odb::dbSet<odb::dbBox> obstructions = master->getObstructions();
                odb::dbSet<odb::dbBox>::iterator boxIter;
                
                for (boxIter = obstructions.begin(); boxIter != obstructions.end(); boxIter++) {
                        odb::dbBox* currBox = *boxIter;
                        int layer = currBox->getTechLayer()->getRoutingLevel();
                        
                        odb::Rect rect;
                        currBox->getBox(rect);
                        transform.apply(rect);

                        Coordinate lowerBound = Coordinate(rect.xMin(), rect.yMin());
                        Coordinate upperBound = Coordinate(rect.xMax(), rect.yMax());
                        Box obstacleBox = Box(lowerBound, upperBound, layer);
                        _grid->addObstacle(layer, obstacleBox);
                }
        }
        
        // Get nets obstructions (routing wires and pdn wires)
        odb::dbSet<odb::dbNet> nets = block->getNets();
        
        if (nets.size() == 0) {
                std::cout << "[ERROR] Design without nets. Exiting...\n";
                std::exit(1);
        }
        
        odb::dbSet<odb::dbNet>::iterator nIter;
        
        for (nIter = nets.begin(); nIter != nets.end(); ++nIter) {
                odb::dbNet* currNet = *nIter;
                
                uint wireCnt = 0, viaCnt = 0;
                currNet->getWireCount(wireCnt, viaCnt);
                if (wireCnt < 1)
                        continue;
                
                if (currNet->getSigType() == odb::dbSigType::POWER ||
                    currNet->getSigType() == odb::dbSigType::GROUND) {
                        odb::dbSet<odb::dbSWire> swires = currNet->getSWires();
                        
                        odb::dbSet<odb::dbSWire>::iterator itr;
                        for (itr = swires.begin(); itr != swires.end(); ++itr) {
                                odb::dbSWire* swire = *itr;
                                odb::dbSet<odb::dbSBox> wires = swire->getWires();
                                odb::dbSet<odb::dbSBox>::iterator box_itr;
                                for (box_itr = wires.begin(); box_itr != wires.end(); ++box_itr) {
                                        odb::dbSBox* s = *box_itr;
                                        if (s->isVia()) {
                                                continue;
                                        } else {
                                                odb::Rect wireRect;
                                                s->getBox(wireRect);
                                                int l = s->getTechLayer()->getRoutingLevel();
                                                
                                                Coordinate lowerBound = Coordinate(wireRect.xMin(), wireRect.yMin());
                                                Coordinate upperBound = Coordinate(wireRect.xMax(), wireRect.yMax());
                                                Box obstacleBox = Box(lowerBound, upperBound, l);
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

}
