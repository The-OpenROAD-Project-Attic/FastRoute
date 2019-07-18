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

       public:
        FastRouteProcess() = default;
        bool run(const Rsyn::Json &params) override;

};  // end class

}  // namespace Rsyn

#endif
