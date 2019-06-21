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

namespace Rsyn {

class FastRouteProcess : public Process {
       private:
	typedef struct {
		long lower_left_x;
		long lower_left_y;
		long tile_width;
		long tile_height;
	} GRID;

        Rsyn::Session session;
        Rsyn::Design design;
        Rsyn::Module module;
	Rsyn::PhysicalDesign phDesign;

	GRID grid;

        // Main functions
	void setXYGridsAndLayers();
        void setCapacities();
        void setSpacingsAndMinWidth();
        void initGrid();
        void initNets();
        // void initEdges(): ?
        void computeAdustments();
        // void initAuxVar(); ?
        void writeGuides();

	//Aux functions
	void getPinPosOnGrid(DBUxy &);

       public:
        FastRouteProcess() = default;
//        FastRoute(const FastRoute &orig) {}
//        virtual ~FastRoute();
        virtual bool run(const Rsyn::Json &params) override;

};  // end class

}

#endif
