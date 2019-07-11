#ifndef FASTROUTE_H
#define FASTROUTE_H
#include <vector>
#include <string>

namespace FastRoute {

typedef struct {
        long x;
        long y;
        int layer;
} PIN;

typedef struct {
        long initX;
        long initY;
        int initLayer;
        long finalX;
        long finalY;
        int finalLayer;
} ROUTE;

typedef struct {
        std::string name;
        int id;
        std::vector<ROUTE> route;
} NET;

class FT {
       public:
        FT() = default;

        void setGridsAndLayers(int x, int y, int nLayers);
        void addVCapacity(int verticalCapacity, int layer);
        void addHCapacity(int horizontalCapacity, int layer);
        void addMinWidth(int width, int layer);
        void addMinSpacing(int spacing, int layer);
        void addViaSpacing(int spacing, int layer);
        void setNumberNets(int nNets);
        void setLowerLeft(int x, int y);
        void setTileSize(int width, int height);
        void addNet(char *name, int netIdx, int nPIns, int minWIdth, PIN pins[]);
        void initEdges();
        void setNumAdjustments(int nAdjustements);
        void addAdjustment(long x1, long y1, int l1, long x2, long y2, int l2, int reducedCap);
        void initAuxVar();
        int run(std::vector<NET> &);
        std::vector<NET> getResults();
};
}  // namespace FastRoute
#endif
