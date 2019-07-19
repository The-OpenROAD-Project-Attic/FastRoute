////////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (c) 2018, Iowa State University All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software
// without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "global.h"
#include "neighbors.h"
#include "dist.h"
#include "heap.h"

namespace FastRoute {
void mst2_package_init(long n) {
        allocate_heap(n);
        allocate_nn_arrays(n);
}

void mst2_package_done() {
        deallocate_heap();
        deallocate_nn_arrays();
}

void mst2(
    long n,
    Point* pt,
    long* parent) {
        long i, k, nn1;
        long d;
        long oct;
        long root = 0;
        extern nn_array* nn;

        //  brute_force_nearest_neighbors( n, pt, nn );
        dq_nearest_neighbors(n, pt, nn);

        /* Binary heap implementation of Prim's algorithm. */
        /* Runs in O(n*log(n)) time since at most 8n edges are considered */

        heap_init(n);
        heap_insert(root, 0);
        parent[root] = root;

        for (k = 0; k < n; k++) /* n points to be extracted from heap */
        {
                i = heap_delete_min();

                if (i < 0) break;
#ifdef DEBUG
                assert(i >= 0);
#endif

                /* pt[i] entered the tree, update heap keys for its neighbors */
                for (oct = 0; oct < 8; oct++) {
                        nn1 = nn[i][oct];
                        if (nn1 >= 0) {
                                d = dist(pt[i], pt[nn1]);
                                if (in_heap(nn1) && (d < heap_key(nn1))) {
                                        heap_decrease_key(nn1, d);
                                        parent[nn1] = i;
                                } else if (never_seen(nn1)) {
                                        heap_insert(nn1, d);
                                        parent[nn1] = i;
                                }
                        }
                }
        }
}

}  // namespace FastRoute
