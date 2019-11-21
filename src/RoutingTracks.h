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

#ifndef __ROUTINGTRACKS_H_
#define __ROUTINGTRACKS_H_

#include <string>
#include <vector>
#include <functional>
#include <limits>
#include <iostream>

#include "Coordinate.h"
#include "Box.h"
#include "Pin.h"

namespace FastRoute {

class RoutingTracks {
private:
        int _layerIndex;
        DBU _space;
        DBU _location;
        int _numTracks;
        bool _orientation;
        
public:
        RoutingTracks() = default;
        RoutingTracks(const int layerIndex, const DBU space,
                      const DBU location, const int numTracks,
                      const bool orientation)
            : _layerIndex(layerIndex), _space(space),
              _location(location), _numTracks(numTracks),
              _orientation(orientation) {}
        
        int getLayerIndex() const { return _layerIndex; }
        DBU getSpace() const { return _space; }
        DBU getLocation() const { return _location; }
        int getNumTracks() const { return _numTracks; }
        bool getOrientation() const { return _orientation; }
};

}

#endif /* __ROUTINGTRACKS_H_ */