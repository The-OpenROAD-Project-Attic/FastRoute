/* Copyright 2014-2018 Rsyn
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "FastRoute.h"
#include "fastroute/fastRoute.h"
#include "phy/PhysicalDesign.h"
#include <cstring>
#include <string>

namespace Rsyn {

bool FastRouteProcess::run(const Rsyn::Json &params) {
        design = session.getDesign();
	module = design.getTopModule();
	phDesign = session.getPhysicalDesign();

	grid.lower_left_x = 0;
	grid.lower_left_y = 0;
	grid.tile_width = 4000;
	grid.tile_height = 4000;

	fastRoute.setTileSize(4000, 4000);

	//initGrid();
	//setCapacities();
        //setSpacingsAndMinWidth();
        initNets();
        
        return 0;
}

void FastRouteProcess::initGrid () {
	int nLayers = 0;
	DBU trackSpacing;	

	for (Rsyn::PhysicalLayer phLayer : phDesign.allPhysicalLayers()){
		if (phLayer.getType() != Rsyn::ROUTING)
			continue;
		nLayers++;
		if (phLayer.getRelativeIndex() != selectedMetal -1)
			continue;

		Rsyn::PhysicalLayerDirection metalDirection = phLayer.getDirection();

		for (Rsyn::PhysicalTracks phTrack : phDesign.allPhysicalTracks(phLayer)){
			if (phTrack.getDirection() == (PhysicalTrackDirection) metalDirection)
				continue;
			trackSpacing = phTrack.getSpace();
			break;
		}
	}

	DBU tileSize = trackSpacing* pitchesInTile;

	Rsyn::PhysicalDie phDie = phDesign.getPhysicalDie();
	Bounds dieBounds = phDie.getBounds();

	DBU dieX = dieBounds[UPPER][X] - dieBounds[LOWER][X];
	DBU dieY = dieBounds[UPPER][Y] - dieBounds[UPPER][Y];
	
	int xGrid = std::ceil(dieX/tileSize);
	int yGrid = std::ceil(dieY/tileSize);
	
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
					hCapacity = (grid.tile_width/phTracks.getSpace());
					break;
				}
			}
			fastRoute.addVCapacity(vCapacity, phLayer.getRelativeIndex());
			fastRoute.addHCapacity(hCapacity, phLayer.getRelativeIndex());
		} else {	
			for (PhysicalTracks phTracks : phDesign.allPhysicalTracks(phLayer)) {
				if (phTracks.getDirection() != (PhysicalTrackDirection)Rsyn::VERTICAL) {
					vCapacity = (grid.tile_width/phTracks.getSpace());
					break;
				}
			}
			fastRoute.addVCapacity(vCapacity, phLayer.getRelativeIndex());
			fastRoute.addHCapacity(hCapacity, phLayer.getRelativeIndex());
		}
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
			fastRoute.addMinSpacing(minSpacing, phLayer.getRelativeIndex());
			fastRoute.addMinWidth(minWidth, phLayer.getRelativeIndex());
			fastRoute.addViaSpacing(1, phLayer.getRelativeIndex());
		}
	}	
}



void FastRouteProcess::initNets() {
	int idx = 0;

	int netsToDebug = 5;
	
	for (Rsyn::Net net : module.allNets()) {
		if (netsToDebug > 0)
			std::cout << "Net name: " << net.getName() << "\n";
		std::vector<FastRoute::PIN> pins;
		for (Rsyn::Pin pin: net.allPins()) {
			if (pin.getInstanceType() != Rsyn::CELL)
				continue;

			DBUxy pinPosition;
			int pinLayer;
			Rsyn::PhysicalLibraryPin phLibPin = phDesign.getPhysicalLibraryPin(pin);
			Rsyn::Cell cell = pin.getInstance().asCell();
			Rsyn::PhysicalCell phCell = phDesign.getPhysicalCell(cell);
			const DBUxy cellPos = phCell.getPosition();
			const Rsyn::PhysicalTransform &transform = phCell.getTransform(true);
			
			for (Rsyn::PhysicalPinGeometry pinGeo : phLibPin.allPinGeometries()) {
				if (!pinGeo.hasPinLayer())
					continue;
				for (Rsyn::PhysicalPinLayer phPinLayer : pinGeo.allPinLayers()) {
					pinLayer = phPinLayer.getLayer().getRelativeIndex();
					
					std::vector<DBUxy> pinBdsPositions;
					DBUxy bdsPosition;
					for (Bounds bds : phPinLayer.allBounds()) {
						bds = transform.apply(bds);
						bds.translate(cellPos);
						bdsPosition = bds.computeCenter();
						getPinPosOnGrid(bdsPosition);
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
			FastRoute::PIN grPin;
			grPin.x = pinPosition.x;
			grPin.y = pinPosition.y;
			grPin.layer = pinLayer;
			pins.push_back(grPin);
		}
		netsToDebug--;
		FastRoute::PIN grPins[pins.size()];
		
		int count = 0;
		char netName[net.getName().size()+1];
		strcpy(netName, net.getName().c_str());
		for (FastRoute::PIN pin : pins) {
			std::cout << "--||grPin pos: (" << pin.x << ", " << pin.y << ")\n";
			std::cout << "--||grPin layer: " << pin.layer << "\n";
			grPins[count] = pin;
			count++;
		}
		
		fastRoute.addNet(netName, idx, pins.size(), 1, grPins);
	}
}

// void FastRouteProcess::initEdges(): ?

void FastRouteProcess::computeAdjustments() {

}

// void FastRoute::initAuxVar(); ?

void FastRouteProcess::writeGuides() {

}

// check when pos in on the edge
void FastRouteProcess::getPinPosOnGrid(DBUxy &pos) {
	DBU x = pos.x;
	DBU y = pos.y;

	// Computing x-center:
	DBU gCellId_X = floor(x/grid.tile_width);
	DBU gCellId_Y = floor(y/grid.tile_height);
	
	DBU centerX = (gCellId_X) * grid.tile_width + (grid.tile_width/2);
	DBU centerY = (gCellId_Y) * grid.tile_height + (grid.tile_height/2);

	pos = DBUxy(centerX, centerY);
}

void FastRouteProcess::setGridAdjustments(){
	Rsyn::PhysicalDie phDie = phDesign.getPhysicalDie();
	Bounds dieBounds = phDie.getBounds();
	DBUxy upperDieBounds = dieBounds[UPPER];
        int xGrids = grid.xGrids;
	int yGrids = grid.yGrids;	
	int xBlocked = upperDieBounds[X] % xGrids;
	int yBlocked = upperDieBounds[Y] % yGrids;
	float percentageBlockedX = xBlocked/grid.tile_width;
	float percentageBlockedY = yBlocked/grid.tile_height;

	for (Rsyn::PhysicalLayer phLayer : phDesign.allPhysicalLayers()){
		if (phLayer.getType() != Rsyn::ROUTING)
			continue;
		int layerN = phLayer.getRelativeIndex();
		int newVCapacity = std::floor(vCapacities[layerN]*percentageBlockedX);
		int newHCapacity = std::floor(hCapacities[layerN]*percentageBlockedY);
		if (percentageBlockedX != 0){
			for (int i=1; i < yGrids; i++){
				fastRoute.addAdjustment(xGrids-1, i-1, layerN, xGrids-1, i, layerN, newVCapacity);
			}
		}
		if (percentageBlockedY != 0){	
			for (int i=1; i < xGrids; i++){
				fastRoute.addAdjustment(i-1, yGrids-1, layerN, i, yGrids-1, layerN, newHCapacity);
			}
		}
	}
}

}
