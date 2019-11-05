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

#include "FastRouteKernel.h"

FastRouteKernel::FastRouteKernel(Parameters& parms)
    : _parms(&parms), _dbWrapper(_grid, parms) {
}

void FastRouteKernel::initGrid() {        
        _dbWrapper.initGrid();
        _dbWrapper.computeCapacities();
        
        fastRoute.setLowerLeft(_grid.getLowerLeftX(), _grid.getLowerLeftY());
        fastRoute.setTileSize(_grid.getTileWidth(), _grid.getTileHeight());
        fastRoute.setGridsAndLayers(_grid.getXGrids(), _grid.getYGrids(), _grid.getNumLayers());
}

void FastRouteKernel::setCapacities() {
        for (int l = 1; l < _grid.getNumLayers(); l++) {
                fastRoute.addHCapacity(_grid.getHorizontalEdgesCapacities()[l-1], l);
                fastRoute.addVCapacity(_grid.getVerticalEdgesCapacities()[l-1], l);
        }
}

void FastRouteKernel::printGrid() {
        std::cout << "**** Global Routing Grid ****\n";
        std::cout << "******** Lower left: (" << _grid.getLowerLeftX() << ", " <<
                    _grid.getLowerLeftY() << ") ********\n";
        std::cout << "******** Tile size: " << _grid.getTileWidth() << " ********\n";
        std::cout << "******** xGrids, yGrids: " << _grid.getXGrids() << ", " <<
                    _grid.getYGrids() << " ********\n";
        std::cout << "******** Perfect regular X/Y: " << _grid.isPerfectRegularX() << "/" <<
                    _grid.isPerfectRegularY() << " ********\n";
        std::cout << "******** Num layers: " << _grid.getNumLayers() << " ********\n";
}

void FastRouteKernel::run() {
        if (!_parms->isInteractiveMode()) {
                std::cout << "Parsing input files...\n";
                _dbWrapper.parseLEF(_parms->getInputLefFile()); 
                _dbWrapper.parseDEF(_parms->getInputDefFile());
                std::cout << "Parsing input files... Done!\n";
        }
        
        std::cout << "Initializing grid...\n";
        initGrid();
        std::cout << "Initializing grid... Done!\n";
        std::cout << "Setting capacities...\n";
        setCapacities();
        std::cout << "Setting capacities... Done!\n";
        printGrid();
}