#include "DBWrapper.h"

#include <iostream>
#include <vector>

#include "db.h"
#include "lefin.h"
#include "defin.h"
#include "defout.h"
#include "dbShape.h"

DBWrapper::DBWrapper() {
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

/*
void DBWrapper::initCore() {
        ads::dbTech* tech = _db->getTech();
        if (!tech) {
                std::cout << "[ERROR] ads::dbTech not initialized! Exiting...\n";
                std::exit(1);
        }

        int databaseUnit = tech->getLefUnits(); 

        ads::dbBlock* block = _chip->getBlock();
        if (!block) {
                std::cout << "[ERROR] ads::dbBlock not found! Exiting...\n";
                std::exit(1);
        }

        ads::dbBox* coreBBox = block->getBBox();

        Coordinate lowerBound(coreBBox->xMin(), coreBBox->yMin());
        Coordinate upperBound(coreBBox->xMax(), coreBBox->yMax());
        
        int horLayerIdx = _parms->getHorizontalMetalLayer();
        int verLayerIdx = _parms->getVerticalMetalLayer();

        ads::dbTechLayer* horLayer = tech->findRoutingLayer(horLayerIdx);
        if (!horLayer) {
                std::cout << "[ERROR] Layer" << horLayerIdx << " not found! Exiting...\n";
                std::exit(1);
        }

        ads::dbTechLayer* verLayer = tech->findRoutingLayer(verLayerIdx);
        if (!horLayer) {
                std::cout << "[ERROR] Layer" << verLayerIdx << " not found! Exiting...\n";
                std::exit(1);
        }

        ads::dbTrackGrid* horTrackGrid = block->findTrackGrid( horLayer );        
        ads::dbTrackGrid* verTrackGrid = block->findTrackGrid( verLayer );
        if (!horTrackGrid || !verTrackGrid) {
                std::cout << "[ERROR] No track grid! Exiting...\n";
                std::exit(1);
        }

        int minSpacingX = 0;
        int minSpacingY = 0;
        int initTrackX = 0;
        int initTrackY = 0;
        int minAreaX = 0;
        int minAreaY = 0;
        int minWidthX = 0;
        int minWidthY = 0;
        
        int numTracks = -1;
        verTrackGrid->getGridPatternX(0, initTrackX, numTracks, minSpacingX);
        horTrackGrid->getGridPatternY(0, initTrackY, numTracks, minSpacingY);

        minAreaX =  verLayer->getArea();
        minWidthX = verLayer->getWidth();
        minAreaY =  horLayer->getArea();
        minWidthY = horLayer->getWidth();

        *_core = Core(lowerBound, upperBound, minSpacingX * 2, minSpacingY * 2,
                      initTrackX, initTrackY, minAreaX, minAreaY,
                      minWidthX, minWidthY, databaseUnit);

        if(_verbose) {
                std::cout << "lowerBound: " << lowerBound.getX() << " " << lowerBound.getY() << "\n";
                std::cout << "upperBound: " << upperBound.getX() << " " << upperBound.getY() << "\n";
                std::cout << "minSpacingX: " << minSpacingX << "\n";
                std::cout << "minSpacingY: " << minSpacingY << "\n";
                std::cout << "initTrackX: " << initTrackX << "\n";
                std::cout << "initTrackY: " << initTrackY << "\n";
                std::cout << "minAreaX: " << minAreaX << "\n";
                std::cout << "minAreaY: " << minAreaY << "\n";
                std::cout << "minWidthX: " << minWidthX << "\n";
                std::cout << "minWidthY: " << minWidthY << "\n";
                std::cout << "databaseUnit: " << databaseUnit << "\n";
        }
}
*/
