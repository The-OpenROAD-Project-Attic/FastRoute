/* Copyright 2014-2017 Rsyn
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/* 
 * File:   PhysicalPinGeometry.h
 * Author: jucemar
 *
 * Created on 18 de Setembro de 2016, 08:50
 */

namespace Rsyn {

inline PhysicalPinGeometryClass PhysicalPinGeometry::getPinGeometryClass() const {
	return data->clsPinPortClass;
} // end method 

// -----------------------------------------------------------------------------

inline bool PhysicalPinGeometry::hasPinLayer() const {
	return !data->clsPinLayers.empty();
} // end method 

// -----------------------------------------------------------------------------

inline std::size_t PhysicalPinGeometry::getNumPinLayers() const{
	return data->clsPinLayers.size();
} // end method 

// -----------------------------------------------------------------------------

inline const std::vector<Rsyn::PhysicalPinLayer> &  PhysicalPinGeometry::allPinLayers() const {
	return data->clsPinLayers;
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalPinLayer PhysicalPinGeometry::getLowerPinLayer() const {
	if(data->clsPinLayers.empty())
		return Rsyn::PhysicalPinLayer(nullptr);
	return data->clsPinLayers.front();
} // end method 

// -----------------------------------------------------------------------------

inline Rsyn::PhysicalPinLayer PhysicalPinGeometry::getUpperPinLayer() const {
	if(data->clsPinLayers.empty())
		return Rsyn::PhysicalPinLayer(nullptr);
	return data->clsPinLayers.back();

} // end method 

// -----------------------------------------------------------------------------

} // end namespace 