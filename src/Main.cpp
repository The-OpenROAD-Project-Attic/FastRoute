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
#include <chrono>
#include <ctime>
#include <tcl.h>

#include "FastRouteKernel.h"

extern "C" {
        extern int Fastroute_Init(Tcl_Interp *interp);
}
int TclAppInit(Tcl_Interp *interp) {
        std::cout << " > Running FastRoute4-lefdef in interactive mode.\n";
        
        Tcl_Init(interp);
        Fastroute_Init(interp);
        
//        std::string command = "";
//
//        command = "";
//        command += "puts \"FastRoute4-lefdef\"";
//        Tcl_Eval(interp, command.c_str());

        return TCL_OK;
}

FastRouteKernel* fastRouteKernel = nullptr;

int main(int argc, char** argv) {
        std::cout << " ######################################\n";
        std::cout << " #      OpenROAD Global Route tool    #\n";
        std::cout << " #                                    #\n";
        std::cout << " # Authors:                           #\n";
        std::cout << " #    Vitor Bandeira (UFRGS)          #\n";
        std::cout << " #    Mateus Fogaca (UFRGS)           #\n";
        std::cout << " #    Eder Matheus Monteiro (UFRGS)   #\n";
        std::cout << " #    Isadora Oliveira (UFRGS)        #\n";
        std::cout << " #                                    #\n";
        std::cout << " #  Advisor:                          #\n";
        std::cout << " #    Ricardo Reis (UFRGS)            #\n";
        std::cout << " ######################################\n";
        std::cout << "\n";

        std::time_t date = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
        std::cout << " > Current time: " << std::ctime(&date);

        //parmsToIOPlacer = new Parameters(argc, argv);
        fastRouteKernel = new FastRouteKernel();
        Tcl_Main(argc, argv, TclAppInit);
/*
        if (parmsToIOPlacer->isInteractiveMode()) {
                Tcl_Main(argc, argv, tclAppInit);
        } else {
                ioPlacerKernel->run();
                ioPlacerKernel->writeDEF();
        }
*/
        return 0;
}
