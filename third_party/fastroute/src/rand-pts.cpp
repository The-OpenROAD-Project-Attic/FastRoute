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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int ac, char *av[]) {
        int d = 10, tmp, i;
        int PNUM = 0;

        for (i = 1; i < ac; i++) {
                if (strcmp(av[i], "-r") == 0)  // random
                        srandom((int)getpid());
                else if (strncmp(av[i], "-s", 2) == 0)  // set random seed
                        srandom(atoi(av[i] + 2));
                else if (strcmp(av[i], "-n") == 0)  // print # of points first
                        PNUM = 1;
                else if (sscanf(av[i], "%d", &tmp))  // set # of points
                        d = tmp;
                else {
                        printf("Usage: %s [-r] [-s<S>] [-n] [<D>]\n", av[0]);
                        printf("  Output <D> random points ");
                        printf("as <D> lines of coordinate pairs.\n");
                        printf("  Default <D> is 10.\n");
                        printf("  -r\t Randomize. Use getpid() as seed.\n");
                        printf("  -s<S>\t Set random seed to <S>.\n");
                        printf("  -n\t Write <D> first before the random points.\n");
                        exit(-1);
                }
        }

        if (PNUM)
                printf("%d\n", d);
        for (i = 1; i <= d; i++)
                printf("%4d %4d\n", (int)random() % 10000, (int)random() % 10000);
}
