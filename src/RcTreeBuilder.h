#ifndef RCTREEBUILDER_H
#define RCTREEBUILDER_H

#include "Net.h"
#include "SteinerTree.h"

namespace sta {
        class Network;
        class Parasitics;
        class Parasitic;
        class ParasiticAnalysisPt;
        
}

namespace FastRoute {

class RcTreeBuilder {
public:
        RcTreeBuilder(Net& net, SteinerTree& steinerTree); 
        void run(); 

protected:
        void initStaData();
        void makeParasiticNetwork();
        void createSteinerNodes();
        void computeCapsAndRes();

        Net*                      _net           = nullptr;
        SteinerTree*              _steinerTree   = nullptr;
        sta::Parasitic*           _parasitic     = nullptr; 
        sta::Parasitics*          _parasitics    = nullptr;
        sta::ParasiticAnalysisPt* _analysisPoint = nullptr; 
        sta::Network*             _network       = nullptr;
};

}

#endif
