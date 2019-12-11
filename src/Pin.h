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

#ifndef __PIN_H_
#define __PIN_H_

#include <string>
#include <vector>
#include <functional>
#include <limits>
#include <iostream>
#include <map>

#include "Coordinate.h"
#include "Box.h"

namespace FastRoute {

class Pin {
private:
        std::string _name;
        Coordinate _position;
        std::vector<int> _layers;
        std::map<int, std::vector<Box>> _boxesPerLayer;
        std::string _netName;
        bool _isPort;
        
        void sortLayers() { std::sort(_layers.begin(), _layers.end()); }
        
public:
        Pin() = default;
        Pin(const std::string& name, Coordinate position,
            const std::vector<int> layers,
            const std::map<int, std::vector<Box>>& boxesPerLayer,
            const std::string& netName, bool isPort)
            : _name(name), _position(position), _layers(layers),
            _boxesPerLayer(boxesPerLayer), _netName(netName),
            _isPort(isPort) { sortLayers(); }
        
        std::string getName() const { return _name; }
        Coordinate getPosition() const { return _position; }
        std::vector<int> getLayers() const { return _layers; }
        int getNumLayers() const { return _layers.size(); }
        int getTopLayer() const { return _layers.back(); }
        std::map<int, std::vector<Box>> getBoxes() const { return _boxesPerLayer; }
        std::string getNetName() const { return _netName; }
        bool isPort() const { return _isPort; }
};

}

#endif /* __PIN_H_ */