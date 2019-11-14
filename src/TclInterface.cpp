////////////////////////////////////////////////////////////////////////////////
// Authors: Vitor Bandeira, Mateus Fogaça, Eder Matheus Monteiro e Isadora
// Oliveira
//          (Advisor: Ricardo Reis)
//
// BSD 3-Clause License
//
// Copyright (c) 2019, Federal University of Rio Grande do Sul (UFRGS)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "TclInterface.h"
#include "FastRouteKernel.h"

extern FastRouteKernel* fastRouteKernel;

void help() {
        std::cout << "Import LEF files:                 import_lef \"path/to/file1.lef path/to/fileN.lef\"\n";
        std::cout << "Import DEF files:                 import_def \"path/to/file1.def path/to/fileN.def\"\n";
        std::cout << "Set output file name:             set_output_file \"outputName\"\n";
        std::cout << "User defined capacity adjustment: set_capacity_adjustment FLOAT\n";
        std::cout << "User defined min routing layer:   set_min_layer INTEGER\n";
        std::cout << "User defined max routing layer:   set_max_layer INTEGER\n";
        std::cout << "Enable unidirection route:        set_unidirectional_routing BOOL\n";
        std::cout << "Run FastRoute4-lefdef flow:       run_fastroute\n";
}

void import_lef(const char* file) {
        std::cout << " > Importing LEF file \"" << file << "\"\n";
        fastRouteKernel->parseLef(file);
}

void import_def(const char* file) {
        std::cout << " > Importing DEF file \"" << file << "\"\n";
        fastRouteKernel->parseDef(file);
}

void set_output_file(const char * file) {
        fastRouteKernel->setOutputFile(file);
}

void set_capacity_adjustment(float adjustment) {
        fastRouteKernel->setAdjustment(adjustment);
}

void set_region_adjustment(int minX, int minY, int maxX, int maxY, int layer, float reductionPercentage) {
        fastRouteKernel->addRegionAdjustment(minX, minY, maxX, maxY,
                                             layer, reductionPercentage);
}

void set_min_layer(int minLayer) {
        fastRouteKernel->setMinRoutingLayer(minLayer);
}

void set_max_layer(int maxLayer) {
        fastRouteKernel->setMaxRoutingLayer(maxLayer);
}

void set_unidirectional_routing(bool unidirRouting) {
        fastRouteKernel->setUnidirectionalRoute(unidirRouting);
}

void start_fastroute() {
        fastRouteKernel->startFastRoute();
}

void run_fastroute() {
        fastRouteKernel->runFastRoute();
}

void run() {
        fastRouteKernel->run();
}

void write_guides() {
        fastRouteKernel->writeGuides();
}