/////////////////////////////////////////////////////////////////////////////
//
// BSD 3-Clause License
//
// Copyright (c) 2019, University of California, San Diego.
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
//
///////////////////////////////////////////////////////////////////////////////


#ifndef DBWRAPPER_h
#define DBWRAPPER_h

#include <string>
#include "Netlist.h"
#include "Grid.h"
#include "RoutingLayer.h"
#include "RoutingTracks.h"

#include "opendb/db.h"
#include "opendb/dbShape.h"
#include "opendb/wOrder.h"
#include "include/FastRoute.h"
#include "antennachecker/AntennaChecker.hh"
#include "opendp/Opendp.h"

#include "db_sta/dbSta.hh"
#include "sta/Clock.hh"
#include "sta/Set.hh"

#include <boost/function_output_iterator.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>

// Forward declaration protects FastRoute code from any
// header file from the DB. FastRoute code keeps independent.
namespace odb{
class dbDatabase;
class dbChip;
class dbTech;
}

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

namespace FastRoute {

class DBWrapper {
private:
        typedef int coord_type;
        typedef bg::cs::cartesian coord_sys_type;
        typedef bg::model::point<coord_type, 2, coord_sys_type> point;
        typedef bg::model::box<point> box;
        typedef std::pair<box, int> value;
        typedef bgi::rtree<value, bgi::quadratic<8,4>> r_tree;
public:        
        DBWrapper() = default;
        DBWrapper(Netlist *netlist, Grid *grid) 
                  : _netlist(netlist),
                  _grid(grid) {
        }
        
        void initGrid(int maxLayer);
        void initRoutingLayers(std::vector<RoutingLayer>& routingLayers);
        void initRoutingTracks(std::vector<RoutingTracks>& allRoutingTracks, int maxLayer, std::map<int, float> layerPitches);
        void computeCapacities(int maxLayer, std::map<int, float> layerPitches);
        void computeSpacingsAndMinWidth(int maxLayer);
        void initNetlist(bool reroute);
        void addNet(odb::dbNet* net, Box dieArea, bool isClock);
        void initClockNets();
        void initObstacles();
        int computeMaxRoutingLayer();
        void getLayerRC(unsigned layerId, float& r, float& c);
        void getCutLayerRes(unsigned belowLayerId, float& r);
        float dbuToMeters(unsigned dbu);
        std::set<int> findTransitionLayers(int maxRoutingLayer);
        std::map<int, odb::dbTechVia*> getDefaultVias(int maxRoutingLayer);
        void commitGlobalSegmentsToDB(std::vector<FastRoute::NET> routing, int maxRoutingLayer);
        int checkAntennaViolations(std::vector<FastRoute::NET> routing, int maxRoutingLayer);
        void insertDiode(odb::dbNet* net, std::string antennaCellName, std::string antennaPinName, odb::dbInst* sinkInst,
                         odb::dbITerm* sinkITerm, std::string antennaInstName, int siteWidth, r_tree& fixedInsts);
        void fixAntennas(std::string antennaCellName, std::string antennaPinName);
        void legalizePlacedCells();
        void getFixedInstances(r_tree& fixedInsts);
        
        void setDB(unsigned idx) { _db = odb::dbDatabase::getDatabase(idx); }
        void setSelectedMetal (int metal) { selectedMetal = metal; }
private:
        std::set<odb::dbNet*> _clockNets;
        int selectedMetal = 3;
        sta::dbSta      *_openSta = nullptr;
        odb::dbDatabase *_db = nullptr;
        odb::dbChip     *_chip = nullptr;
        Netlist         *_netlist = nullptr;
        Grid            *_grid = nullptr;
        bool            _verbose = false;
        antenna_checker::AntennaChecker *_arc = nullptr;
        opendp::Opendp *_opendp = nullptr;

        std::map<std::string, odb::dbNet*> dbNets;
        std::map<std::string, std::vector<std::pair<int, std::vector<odb::dbITerm *>>>> antennaViolations;
        std::vector<odb::dbNet*> dirtyNets;
};

}

#endif
