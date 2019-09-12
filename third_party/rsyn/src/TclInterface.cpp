////////////////////////////////////////////////////////////////////////////////
// Authors: Vitor Bandeira, Eder Matheus Monteiro e Isadora
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

#include "TclInterface.h"
#include "session/Session.h"

extern Rsyn::Session session;
std::vector<std::string> lefFiles;
std::vector<std::string> defFiles;

void set_lef_files(const char * files) {
        size_t pos = 0;
        std::string lefs(files);
        std::string token;
        std::string delimiter = " ";
        
        while ((pos = lefs.find(delimiter)) != std::string::npos) {
                token = lefs.substr(0, pos);
                lefFiles.push_back(token);
                lefs.erase(0, pos + delimiter.length());
        }
        lefFiles.push_back(lefs);
}

void set_def_files(const char * files) {
        size_t pos = 0;
        std::string defs(files);
        std::string token;
        std::string delimiter = " ";
        
        while ((pos = defs.find(delimiter)) != std::string::npos) {
                token = defs.substr(0, pos);
                defFiles.push_back(token);
                defs.erase(0, pos + delimiter.length());
        }
        defFiles.push_back(defs);
}

void parse_input_files() {
        Rsyn::Json params;
        params["lefFiles"] = lefFiles;
        params["defFiles"] = defFiles;
        session.runReader("generic", params);
}

void set_output_file(const char * file) {
        session.setSessionVariable("outfile", file);
}

void set_capacity_adjustment(float adjustment) {
        session.setSessionVariable("adjustment", adjustment);
}

void set_min_layer(int minLayer) {
        session.setSessionVariable("minRoutingLayer", minLayer);
}

void set_max_layer(int maxLayer) {
        session.setSessionVariable("maxRoutingLayer", maxLayer);
}

void set_unidirectional_routing(bool unidirRouting) {
        session.setSessionVariable("unidirectionalRoute", unidirRouting);
}

void run_fastroute() {
        bool success = session.runProcess("rsyn.fastRoute");
}

//void write_guides() {
//        
//}