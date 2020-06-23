#ifndef RCTREEBUILDER_H
#define RCTREEBUILDER_H

#include "DBWrapper.h"
#include "Grid.h"
#include "Net.h"
#include "SteinerTree.h"

namespace sta {
        class Net;
        class Network;
        class Parasitics;
        class Parasitic;
        class ParasiticAnalysisPt;
        class Units;
}

namespace FastRoute {

class RcTreeBuilder {
public:
        RcTreeBuilder(Net& net, SteinerTree& steinerTree, Grid &grid,
                      DBWrapper& dbWrapper);
        void run();
        void reportParasitics();

protected:
        void initStaData();
        void makeParasiticNetwork();
        void createSteinerNodes();
        void computeGlobalParasitics();
        void computeLocalParasitics();

        int findNodeToConnect(const Pin& pin,
                              const std::vector<unsigned>& pinNodes) const;
        unsigned computeDist(const Node& n1, const Node& n2) const;
        unsigned computeDist(const Coordinate& coord, const Node& n) const;
        Coordinate computePinCoordinate(const Pin pin) const;

        Net*                      _net           = nullptr;
        Grid*                     _grid          = nullptr;
        DBWrapper*                _dbWrapper     = nullptr;
        sta::Net*                 _staNet        = nullptr;
        SteinerTree*              _steinerTree   = nullptr;
        sta::Parasitic*           _parasitic     = nullptr;
        sta::Parasitics*          _parasitics    = nullptr;
        sta::ParasiticAnalysisPt* _analysisPoint = nullptr;
        sta::Network*             _network       = nullptr;
        sta::Units*               _units         = nullptr;
        bool                      _debug         = false;
};

}

#endif
