////////////////////////////////////////////////////////////////////////////////
// Authors: Vitor Bandeira, Eder Matheus Monteiro e Isadora
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

#ifndef FASTROUTE
#define FASTROUTE

#include "core/Rsyn.h"
#include "phy/PhysicalDesign.h"
#include "session/Session.h"
#include "session/Process.h"
#include "fastroute/fastRoute.h"
#include <utility>

namespace Rsyn {

class FastRouteProcess : public Process {
       private:
        typedef struct {
                long lower_left_x;
                long lower_left_y;
                long tile_width;
                long tile_height;
                int xGrids;
                int yGrids;
                bool perfect_regular_x;
                bool perfect_regular_y;
        } GRID;

        typedef struct {
                int x;
                int y;
        } TILE;

        Rsyn::Session session;
        Rsyn::Design design;
        Rsyn::Module module;
        Rsyn::PhysicalDesign phDesign;
        FastRoute::FT fastRoute;

        GRID grid;
        std::vector<int> vCapacities;
        std::vector<int> hCapacities;
        int pitchesInTile = 15;
        int selectedMetal = 3;
        std::vector<FastRoute::NET> result;
        std::map<std::string, int> netsDegree;

        // Main functions
        void initGrid();
        void setCapacities();
        void setLayerDirection();
        void setSpacingsAndMinWidth();
        void initNets();
        void setGridAdjustments();
        void computeSimpleAdjustments();
        void computeObstaclesAdjustments();
        void writeGuides(std::vector<FastRoute::NET> &, std::string);

        void writeEst(const std::vector<FastRoute::NET> &, std::string);

        // Aux functions
        void getPosOnGrid(DBUxy &);
        Bounds globalRoutingToBounds(const FastRoute::ROUTE &);
        std::pair<TILE, TILE> getBlockedTiles(const Bounds &, Bounds &, Bounds &);
        int computeTileReduce(const Bounds &, const Bounds &, DBU, bool, bool);
        void getSpecialNetsObstacles(std::map<int, std::vector<Bounds>> &);
        void addRemainingGuides(std::vector<FastRoute::NET> &);
        void mergeBounds(std::vector<Bounds> & guideBds);

       public:
        FastRouteProcess() = default;
        bool run(const Rsyn::Json &params) override;

};  // end class

}  // namespace Rsyn

#endif
