#include "RcTreeBuilder.h"

#include "openroad/OpenRoad.hh"
#include "db_sta/dbSta.hh"
#include "sta/Corner.hh"
#include "sta/Network.hh"
#include "sta/Parasitics.hh"
#include "sta/ParasiticsClass.hh"
#include "sta/Units.hh"


namespace FastRoute {

RcTreeBuilder::RcTreeBuilder(Net& net, SteinerTree& steinerTree, Grid& grid, DBWrapper& dbWrapper) : 
                             _net(&net), _steinerTree(&steinerTree), _grid(&grid),
                             _dbWrapper(&dbWrapper) {
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
       
        // Init units
        _units = dbSta->units(); 
        
        _debug = _net->getName() == "net36";
}

void RcTreeBuilder::run() {
        initStaData();
        makeParasiticNetwork();
        computeGlobalParasitics();
        computeLocalParasitics();

        if (_debug) {
                reportParasitics();
        }
                
}

void RcTreeBuilder::makeParasiticNetwork() {
        _staNet = _network->findNet(_net->getName().c_str());
        _parasitic = _parasitics->makeParasiticNetwork(_staNet, false, _analysisPoint);
}

void RcTreeBuilder::createSteinerNodes() {
        unsigned numSteinerPoints = _steinerTree->getNodes().size();
        for (unsigned snode = 0; snode < numSteinerPoints; ++snode) {
                _parasitics->ensureParasiticNode(_parasitic, _staNet, snode); 
        } 
}

void RcTreeBuilder::computeGlobalParasitics() {
        const std::vector<Node>& nodes = _steinerTree->getNodes();
        
        if (_debug) {
                std::cout << "[DEBUG] Reporting global segments of net " << _net->getName() << "\n";        
        }

        for (const Segment& segment : _steinerTree->getSegments()) {
                const Node& node1 = segment.getFirstNode();
                const Node& node2 = segment.getLastNode();
                unsigned idx1 = std::distance(nodes.begin(), std::find(nodes.begin(), nodes.end(), node1));  
                unsigned idx2 = std::distance(nodes.begin(), std::find(nodes.begin(), nodes.end(), node2)); 
                sta::ParasiticNode *n1 =  _parasitics->ensureParasiticNode(_parasitic, _staNet, idx1);
                sta::ParasiticNode *n2 =  _parasitics->ensureParasiticNode(_parasitic, _staNet, idx2);
                
                float cap = 1.0;
                float res = 1.0;
                if (node1.getPosition() == node2.getPosition()) { // Via
                        cap = 0.0;
                        res = 0.003;
                } else { // Wire
                        // First compute wirelength
                        unsigned dbuWirelength = computeDist(node1, node2);
                        float wirelength = _dbWrapper->dbuToMeters(dbuWirelength);
                        
                        // Then get layer r/c-unit from the DB
                        unsigned layerId = node1.getLayer();
                        float rUnit = 0.0, cUnit = 0.0;
                        _dbWrapper->getLayerRC(layerId, rUnit, cUnit);

                        // Finally multiply
                        res = rUnit * wirelength;
                        cap = cUnit * wirelength;
                        
                        if (_debug) {
                                std::cout << "[DEBUG] Layer ID: " << layerId << " dbuWirelength: " << dbuWirelength;
                                std::cout << std::scientific;
                                std::cout << " rUnit: " << rUnit << " cUnit: " << cUnit << "\n"; 
                                std::cout << "[DEBUG]    wirelength: " << wirelength << " res: " << res << " cap: " << cap << "\n";
                                std::cout << std::fixed;
                        }
                }
              

                _parasitics->incrCap(n1, cap/2.0, _analysisPoint);
                _parasitics->makeResistor(nullptr, n1, n2, res, _analysisPoint);
                _parasitics->incrCap(n2, cap/2.0, _analysisPoint);
        } 
}

unsigned RcTreeBuilder::computeDist(const Node& n1, const Node& n2) const {
        return std::abs(n1.getPosition().getX() - n2.getPosition().getX()) +
               std::abs(n1.getPosition().getY() - n2.getPosition().getY());
}

unsigned RcTreeBuilder::computeDist(const Coordinate& coord, const Node& n) const {
        return std::abs(coord.getX() - n.getPosition().getX()) +
               std::abs(coord.getY() - n.getPosition().getY());
}

void RcTreeBuilder::computeLocalParasitics() {
        std::vector<Node> nodes = _steinerTree->getNodes();
        std::vector<unsigned> pinNodes;
        for (unsigned nodeId = 0; nodeId < nodes.size(); ++nodeId) {
                const Node& node = nodes[nodeId];
                if (node.getType() == SINK || node.getType() == SOURCE) {
                        pinNodes.push_back(nodeId);
                }
        }

        if (_debug) {
                std::cout << "[DEBUG] Reporting local segments of net " << _net->getName() << "\n";        
        }

        const std::vector<Pin>& pins = _net->getPins();
        for (const Pin &pin : pins) {
                // Sta pin
                sta::Pin* staPin = _network->findPin(pin.getName().c_str());
                sta::ParasiticNode* n1 = _parasitics->ensureParasiticNode(_parasitic, staPin);

                int nodeToConnect = findNodeToConnect(pin, pinNodes);
                sta::ParasiticNode* n2 = _parasitics->ensureParasiticNode(_parasitic, _staNet, nodeToConnect);        
               
                Coordinate pinCoord = computePinCoordinate(pin); 
               
                unsigned dbuWirelength = computeDist(pinCoord, nodes[nodeToConnect]);
                float wirelength = _dbWrapper->dbuToMeters(dbuWirelength);
                
                // Then get layer r/c-unit from the DB
                unsigned layerId = pin.getTopLayer();
                float rUnit = 0.0, cUnit = 0.0;
                _dbWrapper->getLayerRC(layerId, rUnit, cUnit);

                // Finally multiply
                float res = rUnit * wirelength;
                float cap = cUnit * wirelength;

                if (_debug) {
                        std::cout << "[DEBUG] Layer ID: " << layerId << " dbuWirelength: " << dbuWirelength;
                        std::cout << std::scientific;
                        std::cout << " rUnit: " << rUnit << " cUnit: " << cUnit << "\n"; 
                        std::cout << "[DEBUG]    wirelength: " << wirelength << " res: " << res << " cap: " << cap << "\n";
                        std::cout << std::fixed;
                }

                _parasitics->incrCap(n1, cap/2.0, _analysisPoint);
                _parasitics->makeResistor(nullptr, n1, n2, res, _analysisPoint);
                _parasitics->incrCap(n2, cap/2.0, _analysisPoint);
        }
}      

Coordinate RcTreeBuilder::computePinCoordinate(const Pin pin) const {
        unsigned topLayer = pin.getTopLayer();
        std::vector<Box> pinBoxes = pin.getBoxes()[topLayer];
        unsigned x = 0;
        unsigned y = 0;
        for (Box pinBox : pinBoxes) {
               x += pinBox.getMiddle().getX(); 
               y += pinBox.getMiddle().getY(); 
        }
        x /= pinBoxes.size();
        y /= pinBoxes.size();

        return Coordinate(x, y);         
}

int RcTreeBuilder::findNodeToConnect(const Pin& pin, const std::vector<unsigned>& pinNodes) const {
        std::vector<Node> nodes = _steinerTree->getNodes();
        const std::vector<Pin>& pins = _net->getPins();
        unsigned topLayer = pin.getTopLayer();
        std::vector<Box> pinBoxes = pin.getBoxes()[topLayer];
        for (Box pinBox : pinBoxes) {
                Coordinate posOnGrid = _grid->getPositionOnGrid(pinBox.getMiddle());
                for (unsigned pinNode : pinNodes) {
                        const Node& node = nodes[pinNode];
                        if (posOnGrid == node.getPosition() && node.getLayer() == topLayer) {
                                return pinNode;                
                        } 
                }
        }

        return -1;
}


void RcTreeBuilder::reportParasitics() {
        std::cout << "Net: " << _net->getName() << "\n";
        std::cout << "Num pins:  " << _net->getNumPins() << "\n";

        std::cout << "Nodes: \n";
        const std::vector<Node>& nodes = _steinerTree->getNodes();
        for (unsigned nodeId = 0; nodeId < nodes.size(); ++nodeId) {
                const Node& node = nodes[nodeId];
                std::cout << nodeId << " (" << node.getPosition().getX() << ", " 
                         << node.getPosition().getY() << ", " << node.getLayer() << ") ";
                if (node.getType() == SINK) {
                        std::cout << "sink\n";
                } else if (node.getType() == SOURCE) {
                        std::cout << "source\n";
                } else {
                        std::cout << "steiner\n";
                }
        }

        std::cout << "Segments: \n";
        for (const Segment& segment : _steinerTree->getSegments()) {
                const Node& node1 = segment.getFirstNode();
                const Node& node2 = segment.getLastNode();
                unsigned idx1 = std::distance(nodes.begin(), std::find(nodes.begin(), nodes.end(), node1)); 
                unsigned idx2 = std::distance(nodes.begin(), std::find(nodes.begin(), nodes.end(), node2));
                std::cout << idx1 << " -> " << idx2 << "\n";
                
        }

        std::cout << std::scientific;
        std::cout << "Resistors: \n";
        sta::ParasiticDeviceIterator *deviceIt = _parasitics->deviceIterator(_parasitic);
        while (deviceIt->hasNext()) {
                sta::ParasiticDevice *device = deviceIt->next();
                if (!_parasitics->isResistor(device)) {
                        continue;
                }
                
                sta::ParasiticNode* node1 = _parasitics->node1(device);
                sta::ParasiticNode* node2 = _parasitics->node2(device);
                std::cout << _parasitics->name(node1) << " " << _parasitics->name(node2) << " " ;
                std::cout << _units->resistanceUnit()->asString( _parasitics->value(device, _analysisPoint)) << "\n";        
        }
        
        std::cout << "Capacitors: \n";
        sta::ParasiticNodeIterator *nodeIt = _parasitics->nodeIterator(_parasitic);
        while (nodeIt->hasNext()) {
                sta::ParasiticNode *node = nodeIt->next();
                
                std::cout << _parasitics->name(node) << " " ;
                std::cout << _units->capacitanceUnit()->asString(_parasitics->nodeGndCap(node, _analysisPoint)) << "\n";        
        }
        
        std::cout << std::fixed;
}

}
