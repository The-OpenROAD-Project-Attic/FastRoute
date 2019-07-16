#ifndef _ROUTE_H_
#define _ROUTE_H_

namespace FastRoute {

#define SAMEX 0
#define SAMEY 1

// old functions for segment list data structure
extern void routeSegL(Segment *seg);
extern void routeLAll(Bool firstTime);
// new functions for tree data structure
extern void newrouteL(int netID, RouteType ripuptype, Bool viaGuided);
extern void newrouteZ(int netID, int threshold);
extern void newrouteZ_edge(int netID, int edgeID);
extern void newrouteLAll(Bool firstTime, Bool viaGuided);
extern void newrouteZAll(int threshold);
extern void routeMonotonicAll(int threshold);
extern void routeMonotonic(int netID, int edgeID, int threshold);
extern void routeLVAll(int threshold, int expand);
extern void spiralRouteAll();
extern void newrouteLInMaze(int netID);
}  // namespace FastRoute
#endif /* _ROUTE_H_ */
