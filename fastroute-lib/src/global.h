#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdio.h>

namespace FastRoute {

#define TRUE 1
#define FALSE 0
#define MAXLONG 0x7fffffffL

struct point {
        long x, y;
};

typedef struct point Point;

typedef long nn_array[8];

}  // namespace FastRoute
#endif /* __GLOBAL_H__ */
