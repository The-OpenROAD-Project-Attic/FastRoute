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

#include "Netlist.h"

namespace FastRoute {

Net Netlist::getNetByName(std::string name) {
        Net choosenNet;
        for (Net net : _nets) {
                if (net.getName() == name) {
                        choosenNet = net;
                }
        }
        
        return choosenNet;
}

void Netlist::addNet(const std::string& name, const std::string& signalType, const std::vector<Pin>& pins) {
        Net net = Net(name, signalType, pins);
        _nets.push_back(net);
        _netCount++;
}
        
int Netlist::getMaxNetDegree() {
        if (_nets.size() < 1) {
                std::cout << "[ERROR] Netlist not initialized yet. Exiting...\n";
                std::exit(1);
        }
    
        int maxDegree = -1;
        
        for (Net net : _nets) {
                int netDegree = net.getNumPins();
                if (netDegree > maxDegree) {
                        maxDegree = netDegree;
                }
        }
        
        return maxDegree;
}

std::vector<Pin> Netlist::getAllPorts() {
        std::vector<Pin> ports; 
        for (Net net : _nets) {
                for (Pin pin : net.getPins()) {
                        if (pin.isPort()) {
                                ports.push_back(pin);
                        }
                }
        }
        
        return ports;
}

}