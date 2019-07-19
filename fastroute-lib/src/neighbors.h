#ifndef __NEIGHBORS_H__
#define __NEIGHBORS_H__
#include "global.h"

namespace FastRoute {

void allocate_nn_arrays(long n);
void deallocate_nn_arrays();

void brute_force_nearest_neighbors(
    long n,
    Point* pt,
    nn_array* nn);

void dq_nearest_neighbors(
    long n,
    Point* pt,
    nn_array* nn);

}  // namespace FastRoute
#endif /* __NEIGHBORS_H__ */
