#ifndef __MST2_H__
#define __MST2_H__

#include "global.h"

namespace FastRoute {

void mst2_package_init(long n);
void mst2_package_done();
void mst2(long n, Point* pt, long* parent);

}  // namespace FastRoute

#endif /* __MST2_H__ */
