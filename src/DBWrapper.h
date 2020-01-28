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


#ifndef DBWRAPPER_h
#define DBWRAPPER_h

#include <string>
#include "Netlist.h"
#include "Grid.h"
#include "RoutingLayer.h"
#include "RoutingTracks.h"

#include "opendb/db.h"
#include "opendb/dbShape.h"

// Forward declaration protects FastRoute code from any
// header file from the DB. FastRoute code keeps independent.
namespace odb{
class dbDatabase;
class dbChip;
class dbTech;
}

namespace FastRoute {

class DBWrapper {
public:        
        DBWrapper() = default;
        DBWrapper(Netlist& netlist, Grid& grid) 
                  : _netlist(&netlist),
                  _grid(&grid) {
        }
        
        void initGrid(int maxLayer);
        void initRoutingLayers(std::vector<RoutingLayer>& routingLayers,int maxLayer);
        void initRoutingTracks(std::vector<RoutingTracks>& allRoutingTracks, int maxLayer);
        void computeCapacities(int maxLayer);
        void computeSpacingsAndMinWidth(int maxLayer);
        void initNetlist();
        void initObstacles();
        int computeMaxRoutingLayer();
        
        void setDB(unsigned idx) { _db = odb::dbDatabase::getDatabase(idx); }
        void setSelectedMetal (int metal) { selectedMetal = metal; }
private:
        int selectedMetal = 3;
        odb::dbDatabase *_db;
        odb::dbChip     *_chip;
        Netlist         *_netlist = nullptr;
        Grid            *_grid = nullptr;
        bool            _verbose = false;
};

}

#endif
