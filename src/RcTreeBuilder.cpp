#include "RcTreeBuilder.h"

#include "openroad/OpenRoad.hh"
#include "db_sta/dbSta.hh"
#include "sta/Corner.hh"
#include "sta/Network.hh"
#include "sta/Parasitics.hh"

namespace FastRoute {

RcTreeBuilder::RcTreeBuilder(Net& net, SteinerTree& steinerTree) : 
                             _net(&net), _steinerTree(&steinerTree) {
}

void RcTreeBuilder::initStaData() {
        ord::OpenRoad* openRoad = ord::OpenRoad::openRoad();
        sta::dbSta* dbSta = openRoad->getSta();
        
        // Init analysis point
        sta::Corner* corner = dbSta->corners()->findCorner(0);
        if (!corner) {
                std::cout << "[ERROR] Corner not found!\n";
                std::exit(1);
        }
        sta::MinMax* minMax = sta::MinMax::max();
        _analysisPoint = corner->findParasiticAnalysisPt(minMax);

        // Init parasitics
        _parasitics = dbSta->parasitics();

        // Init network
        _network = dbSta->network();
}

void RcTreeBuilder::run() {
        std::cout << _net->getName() << "\n";
        initStaData();
        makeParasiticNetwork();
        createSteinerNodes();
}

void RcTreeBuilder::makeParasiticNetwork() {
        sta::Net* staNet = _network->findNet(_net->getName().c_str());
        _parasitic = _parasitics->makeParasiticNetwork(staNet, false, _analysisPoint);
}

void RcTreeBuilder::createSteinerNodes() {

}

void RcTreeBuilder::computeCapsAndRes() {

}

}
