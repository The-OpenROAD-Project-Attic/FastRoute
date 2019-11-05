#include "DBWrapper.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>
#include <string>
#include <utility>

#include "db.h"
#include "lefin.h"
#include "defin.h"
#include "defout.h"
#include "dbShape.h"

DBWrapper::DBWrapper(Grid& grid, Parameters& parms) :
                _grid(&grid), _parms(&parms) {
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
                metal1Orientation = 0;
        } else if (layer1->getDirection().getString() == "VERTICAL") {
                metal1Orientation = 1;
        } else {
                std::cout << "[ERROR] Layer 1 does not have valid direction! Exiting...\n";
                std::exit(1);
        }
        
        std::vector<int> genericVector(numLayers);
        
        *_grid = Grid(lowerLeftX, lowerLeftY, tileWidth, tileHeight, xGrids,
                     yGrids, perfectRegularX, perfectRegularY, numLayers,
                     metal1Orientation, genericVector, genericVector,
                     genericVector, genericVector);
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
                
                std::cout << "[DEBUG] Layer " << l << " has min width equal to " << minWidth << "\n";
                _grid->addSpacing(minSpacing, l-1);
                _grid->addMinWidth(minWidth, l-1);
        }
}