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

#ifndef __GRID_H_
#define __GRID_H_

#include <vector>
#include <cmath>
#include <map>
#include <iostream>
#include "Coordinate.h"
#include "Box.h"
#include "RoutingLayer.h"

namespace FastRoute {

class Grid {
private:
        long _lowerLeftX;
        long _lowerLeftY;
        long _upperRightX;
        long _upperRightY;
        long _tileWidth;
        long _tileHeight;
        int _xGrids;
        int _yGrids;
        bool _perfectRegularX;
        bool _perfectRegularY;
        int _numLayers;
        int _pitchesInTile = 15;
        std::vector<int> _spacings;
        std::vector<int> _minWidths;
        std::vector<int> _horizontalEdgesCapacities;
        std::vector<int> _verticalEdgesCapacities;
        std::map<int, std::vector<Box>> _obstacles;
        
public:
        Grid () = default;
        
        Grid(const long lowerLeftX, const long lowerLeftY,
             const long upperRightX, const long upperRightY,
             const long tileWidth, const long tileHeight,
             const int xGrids, const int yGrids,
             const bool perfectRegularX, const bool perfectRegularY,
             const int numLayers, const std::vector<int> spacings,
             const std::vector<int> minWidths,
             const std::vector<int> horizontalCapacities,
             const std::vector<int> verticalCapacities,
             const std::map<int, std::vector<Box>> obstacles)
            : _lowerLeftX(lowerLeftX), _lowerLeftY(lowerLeftY),
              _upperRightX(upperRightX), _upperRightY(upperRightY),
              _tileWidth(tileWidth), _tileHeight(tileHeight),
              _xGrids(xGrids), _yGrids(yGrids),
              _perfectRegularX(perfectRegularX),
              _perfectRegularY(perfectRegularY),
              _numLayers(numLayers), _spacings(spacings), _minWidths(minWidths),
              _horizontalEdgesCapacities(horizontalCapacities),
              _verticalEdgesCapacities(verticalCapacities),
              _obstacles(obstacles) {}
        
        typedef struct {
                int _x;
                int _y;
        } TILE;
        
        long getLowerLeftX() const { return _lowerLeftX; }
        long getLowerLeftY() const { return _lowerLeftY; }
        
        long getUpperRightX() const { return _upperRightX; }
        long getUpperRightY() const { return _upperRightY; }
        
        long getTileWidth() const { return _tileWidth; }
        long getTileHeight() const { return _tileHeight; }
        
        int getXGrids() const { return _xGrids; }
        int getYGrids() const { return _yGrids; }
        
        bool isPerfectRegularX() const { return _perfectRegularX; }
        bool isPerfectRegularY() const { return _perfectRegularY; }
        
        int getNumLayers() const { return _numLayers; }
        
        void setPitchesInTile(const int pitchesInTile) { _pitchesInTile = pitchesInTile; }
        int getPitchesInTile() const { return _pitchesInTile; }
        
        std::vector<int> getSpacings() const { return _spacings; }
        std::vector<int> getMinWidths() const { return _minWidths; }
        
        void addSpacing(int value, int layer) { _spacings[layer] = value; }
        void addMinWidth(int value, int layer) { _minWidths[layer] = value; }
        
        std::vector<int> getHorizontalEdgesCapacities() { return _horizontalEdgesCapacities; };
        std::vector<int> getVerticalEdgesCapacities() { return _verticalEdgesCapacities; };
        
        void addHorizontalCapacity(int value, int layer) { _horizontalEdgesCapacities[layer] = value; }
        void addVerticalCapacity(int value, int layer) { _verticalEdgesCapacities[layer] = value; }
        
        std::map<int, std::vector<Box>> getAllObstacles() const { return _obstacles; }
        void addObstacle(int layer, Box obstacle) { _obstacles[layer].push_back(obstacle); }
        
        Coordinate getPositionOnGrid(const Coordinate& position);
        
        std::pair<TILE, TILE> getBlockedTiles(const Box& obstacle, Box& firstTileBds,
                                              Box& lastTileBds);
        
        int computeTileReduce(const Box &obs, const Box &tile, int trackSpace, bool first, bool direction);
};

}

#endif /* __GRID_H_ */