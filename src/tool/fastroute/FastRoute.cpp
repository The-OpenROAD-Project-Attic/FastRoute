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
        setXYGridsAndLayers();
        setCapacities();
        setSpacingsAndMinWidth();
        initGrid();
        initNets();
        
        return 0;
}

void FastRouteProcess::setXYGridsAndLayers() {
	FastRoute::setGridsAndLayers(10, 10, 9);
}

void FastRouteProcess::setCapacities() {

}

void FastRouteProcess::setSpacingsAndMinWidth() {

}

void FastRouteProcess::initGrid() {

}

void FastRouteProcess::initNets() {
	int idx = 0;
	for (Rsyn::Net net : module.allNets()) {
		std::vector<FastRoute::PIN> pins;
		for (Rsyn::Pin pin: net.allPins()) {
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
					int layer = phPinLayer.getLayer().getRelativeIndex();
					for (Bounds bds : phPinLayer.allBounds()) {
						bds = transform.apply(bds);
						bds.translate(cellPos);
						pinPosition = bds.computeCenter();
						getPinPosOnGrid(pinPosition);
						break;
					}
					break;
				}
				break;
			}
			FastRoute::PIN grPin;
			grPin.x = pinPosition.x;
			grPin.y = pinPosition.y;
			grPin.layer = pinLayer;
		}
		FastRoute::PIN grPins[pins.size()];
		
		int count = 0;
		char netName[net.getName().size()+1];
		strcpy(netName, net.getName().c_str());
		for (FastRoute::PIN pin : pins) {
			grPins[count] = pin;
			count++;
		}

		FastRoute::addNet(netName, idx, pins.size(), 1, grPins);
	}
}

// void FastRouteProcess::initEdges(): ?

void FastRouteProcess::computeAdustments() {

}

// void FastRoute::initAuxVar(); ?

void FastRouteProcess::writeGuides() {

}

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

}
