#include "DBWrapper.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>
#include <string>
#include <utility>
#include <map>

#include "db.h"
#include "lefin.h"
#include "defin.h"
#include "defout.h"
#include "dbShape.h"
#include "Coordinate.h"
#include "Box.h"
#include "Pin.h"

DBWrapper::DBWrapper(Netlist& netlist, Grid& grid, Parameters& parms) :
                _netlist(&netlist), _grid(&grid), _parms(&parms) {
        _db = odb::dbDatabase::create();
}

void DBWrapper::parseLEF(const std::string &filename) {
        odb::lefin lefReader(_db, false);
        lefReader.createTechAndLib("testlib", filename.c_str());
}

void DBWrapper::parseDEF(const std::string &filename) {
        odb::defin defReader(_db);

        std::vector<odb::dbLib *> searchLibs;
        odb::dbSet<odb::dbLib> libs = _db->getLibs();
        odb::dbSet<odb::dbLib>::iterator itr;
        for(itr = libs.begin(); itr != libs.end(); ++itr) {
                searchLibs.push_back(*itr);
        }
        
        _chip = defReader.createChip(searchLibs, filename.c_str());
}

void DBWrapper::initGrid() {
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
        
        odb::dbBox* coreBBox = block->getBBox();
        odb::dbTechLayer* selectedLayer = tech->findRoutingLayer(selectedMetal);
        
        if (!selectedLayer) {
                std::cout << "[ERROR] Layer" << selectedMetal << " not found! Exiting...\n";
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
        
        if (selectedLayer->getDirection().getString() == "HORIZONTAL") {
                trackSpacing = trackStepY;
        } else if (selectedLayer->getDirection().getString() == "VERTICAL") {
                trackSpacing = trackStepX;
        } else {
                std::cout << "[ERROR] Layer " << selectedMetal << " does not have valid direction! Exiting...\n";
                std::exit(1);
        }
        
        long lowerLeftX = coreBBox->xMin();
        long lowerLeftY = coreBBox->yMin();
        
        long upperRightX = coreBBox->xMax() - lowerLeftX;
        long upperRightY = coreBBox->yMax() - lowerLeftY;
        
        long tileWidth = _parms->getPitchesInTile() * trackSpacing;
        long tileHeight = _parms->getPitchesInTile() * trackSpacing;
        
        int xGrids = std::floor((float)upperRightX / tileWidth);
        int yGrids = std::floor((float)upperRightY / tileHeight);
        
        bool perfectRegularX = false;
        bool perfectRegularY = false;
        
        int numLayers = tech->getRoutingLayerCount();
        
        if ((xGrids * tileWidth) == upperRightX)
                perfectRegularX = true;
        
        if ((yGrids * tileHeight) == upperRightY)
                perfectRegularY = true;
        
        bool metal1Orientation = 0;
        
        odb::dbTechLayer* layer1 = tech->findRoutingLayer(1);
        
        if (layer1->getDirection().getString() == "HORIZONTAL") {
                metal1Orientation = RoutingLayer::HORIZONTAL;
        } else if (layer1->getDirection().getString() == "VERTICAL") {
                metal1Orientation = RoutingLayer::VERTICAL;
        } else {
                std::cout << "[ERROR] Layer 1 does not have valid direction! Exiting...\n";
                std::exit(1);
        }
        
        std::vector<int> genericVector(numLayers);
        std::map<int, std::vector<Box>> genericMap;
        
        *_grid = Grid(lowerLeftX, lowerLeftY, coreBBox->xMax(), coreBBox->yMax(),
                     tileWidth, tileHeight, xGrids, yGrids, perfectRegularX,
                     perfectRegularY, numLayers, genericVector, genericVector,
                     genericVector, genericVector, genericMap);
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
                if (techLayer->getDirection().getString() == "HORIZONTAL") {
                        preferredDirection = RoutingLayer::HORIZONTAL;
                } else if (techLayer->getDirection().getString() == "VERTICAL") {
                        preferredDirection = RoutingLayer::VERTICAL;
                } else {
                        std::cout << "[ERROR] Layer 1 does not have valid direction! Exiting...\n";
                        std::exit(1);
                }
                
                RoutingLayer routingLayer = RoutingLayer(index, name, preferredDirection);
                routingLayers.push_back(routingLayer);
        }
}

void DBWrapper::initRoutingTracks(std::vector<RoutingTracks>& allRoutingTracks) {
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

                if (techayer->getDirection().getString() == "HORIZONTAL") {
                        spacing = trackStepY;
                        location = initTrackY;
                        numTracks = numTracksY;
                        orientation = RoutingLayer::HORIZONTAL;
                } else if (techayer->getDirection().getString() == "VERTICAL") {
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

void DBWrapper::computeCapacities() {
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
                odb::dbTechLayer* techLayer = tech->findRoutingLayer(l);
                
                odb::dbTrackGrid* track = block->findTrackGrid(techLayer);
                
                if (!track) {
                        std::cout << "[ERROR] Track for layer " << l << " not found! Exiting...\n";
                        std::exit(1);
                }
                
                track->getGridPatternX(0, initTrackX, numTracksX, trackStepX);
                track->getGridPatternY(0, initTrackY, numTracksY, trackStepY);
                
                if (techLayer->getDirection().getString() == "HORIZONTAL") {
                        trackSpacing = trackStepY;
                        hCapacity = std::floor((float)_grid->getTileWidth() / trackSpacing);
                        
                        _grid->addHorizontalCapacity(hCapacity, l-1);
                        _grid->addVerticalCapacity(0, l-1);
                } else if (techLayer->getDirection().getString() == "VERTICAL") {
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

void DBWrapper::computeSpacingsAndMinWidth() {
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
                odb::dbTechLayer* techLayer = tech->findRoutingLayer(l);
                
                odb::dbTrackGrid* track = block->findTrackGrid(techLayer);
                
                if (!track) {
                        std::cout << "[ERROR] Track for layer " << l << " not found! Exiting...\n";
                        std::exit(1);
                }
                
                track->getGridPatternX(0, initTrackX, numTracksX, trackStepX);
                track->getGridPatternY(0, initTrackY, numTracksY, trackStepY);
                
                if (techLayer->getDirection().getString() == "HORIZONTAL") {
                        minWidth = trackStepY;
                } else if (techLayer->getDirection().getString() == "VERTICAL") {
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
                std::vector<Pin> netPins;
                
                odb::dbNet* currNet = *nIter;
                if (currNet->getSigType().getString() == "POWER" ||
                    currNet->getSigType().getString() == "GROUND") {
                        continue;
                }
                std::string netName =currNet->getConstName();
                
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
                        std::string instName = currITerm->getInst()->getConstName();
                        pinName = mTerm->getConstName();
                        pinName = instName + ":" + pinName;
                        
                        odb::dbSet<odb::dbMPin> mTermPins = mTerm->getMPins();
                        odb::dbSet<odb::dbMPin>::iterator pinIter;
                        
                        odb::dbInst* inst = currITerm->getInst();
                        inst->getOrigin(pX, pY);
                        odb::adsPoint origin = odb::adsPoint(pX, pY);
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
                                        odb::adsRect rect;
                                        box->getBox(rect);
                                        transform.apply(rect);
                                        
                                        odb::dbTechLayer* techLayer = box->getTechLayer();
                                        if (techLayer->getType().getString() != "ROUTING") {
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
                                
                                Pin pin = Pin(pinName, pinLayers, pinBoxes, netName);
                                netPins.push_back(pin);
                        }
                }
                
                // Iterate through all I/O pins
                odb::dbSet<odb::dbBTerm> bTerms = currNet->getBTerms();
                odb::dbSet<odb::dbBTerm>::iterator bIter;
                
                for (bIter = bTerms.begin(); bIter != bTerms.end(); bIter++) {
                        odb::dbBTerm* currBTerm = *bIter;
                        std::string pinName;
                        
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
                                if (techLayer->getType().getString() != "ROUTING") {
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
                        
                        Pin pin = Pin(pinName, pinLayers, pinBoxes, netName);
                        netPins.push_back(pin);
                }
                _netlist->addNet(netName, netPins);
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
        int instObs = 0;
        odb::dbSet<odb::dbInst> insts;
        insts = block->getInsts();
        
        odb::dbSet<odb::dbInst>::iterator instIter;
        
        for (instIter = insts.begin(); instIter != insts.end(); instIter++) {
                int pX, pY;
                odb::dbInst* currInst = *instIter;
                odb::dbMaster* master = currInst->getMaster();
                
                currInst->getOrigin(pX, pY);
                odb::adsPoint origin = odb::adsPoint(pX, pY);
                
                odb::dbTransform transform(currInst->getOrient(), origin);
                
                odb::dbSet<odb::dbBox> obstructions = master->getObstructions();
                odb::dbSet<odb::dbBox>::iterator boxIter;
                
                for (boxIter = obstructions.begin(); boxIter != obstructions.end(); boxIter++) {
                        odb::dbBox* currBox = *boxIter;
                        int layer = currBox->getTechLayer()->getRoutingLevel();
                        
                        odb::adsRect rect;
                        currBox->getBox(rect);
                        transform.apply(rect);

                        Coordinate lowerBound = Coordinate(rect.xMin(), rect.yMin());
                        Coordinate upperBound = Coordinate(rect.xMax(), rect.yMax());
                        Box obstacleBox = Box(lowerBound, upperBound, layer);
                        _grid->addObstacle(layer, obstacleBox);
                }
        }
}