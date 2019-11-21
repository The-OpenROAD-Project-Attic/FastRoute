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

#ifndef __PARAMETERS_H_
#define __PARAMETERS_H_

#include <string>
#include <vector>

namespace FastRoute {

class Parameters {
private:
        std::string _inputLefFile;
        std::string _inputDefFile;
        std::string _outputFile;
        
        float _adjustment = 0.0;
        int _minRoutingLayer = 1;
        int _maxRoutingLayer = -1;
        bool _unidirectionalRoute = false;
        int _pitchesInTile = 15;
        bool _interactiveMode = false;
        
        // Region adjustment variables
        std::vector<int> _regionsMinX;
        std::vector<int> _regionsMinY;
        std::vector<int> _regionsMaxX;
        std::vector<int> _regionsMaxY;
        std::vector<int> _regionsLayer;
        std::vector<float> _regionsReductionPercentage;
        
public:
        Parameters() = default;
        Parameters(int, char**);
        
        void setInputLefFile(const std::string& file) { _inputDefFile = file; }
        std::string getInputLefFile() const { return _inputLefFile; }
        
        
        void setInputDefFile(const std::string& file) { _inputDefFile = file; }
        std::string getInputDefFile() const { return _inputDefFile; }
        
        void setOutputFile(const std::string& file) { _outputFile = file; }
        std::string getOutputFile() const { return _outputFile; }
        
        void setAdjustment(const float adjustment) { _adjustment = adjustment; }
        float getAdjustment() const { return _adjustment; }
        
        void setMinRoutingLayer(const int minLayer) { _minRoutingLayer = minLayer; }
        int getMinRoutingLayer() const { return _minRoutingLayer; }
        
        void setMaxRoutingLayer(const int maxLayer) { _maxRoutingLayer = maxLayer; }
        int getMaxRoutingLayer() const { return _maxRoutingLayer; }
        
        void setUnidirectionalRoute(const bool unidirRoute) { _unidirectionalRoute = unidirRoute; }
        bool getUnidirectionalRoute() const { return _unidirectionalRoute; }
        
        void setPitchesInTile(const bool pitchesInTile) { _pitchesInTile = pitchesInTile; }
        int getPitchesInTile() const { return _pitchesInTile; }
        
        void setInteractiveMode(bool enable) { _interactiveMode = enable; }
        bool isInteractiveMode() const { return _interactiveMode; }
        
        void printAll() const;
};

}

#endif /* __PARAMETERS_H_ */