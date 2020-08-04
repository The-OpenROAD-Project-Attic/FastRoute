/////////////////////////////////////////////////////////////////////////////
//
// BSD 3-Clause License
//
// Copyright (c) 2019, University of California, San Diego.
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
//
///////////////////////////////////////////////////////////////////////////////

#include "Box.h"

namespace FastRoute {

Coordinate Box::getMiddle()
{
  DBU lowerX = _lowerBound.getX();
  DBU lowerY = _lowerBound.getY();
  DBU upperX = _upperBound.getX();
  DBU upperY = _upperBound.getY();

  return Coordinate((lowerX + (upperX - lowerX) / 2.0),
                    (lowerY + (upperY - lowerY) / 2.0));
}

DBU Box::getHalfPerimeter()
{
  DBU lowerX = _lowerBound.getX();
  DBU lowerY = _lowerBound.getY();
  DBU upperX = _upperBound.getX();
  DBU upperY = _upperBound.getY();

  DBU x = upperX - lowerX;
  DBU y = upperY - lowerY;

  return (x + y);
}

bool Box::overlap(Box box)
{
  const DBU dx = std::max(_lowerBound.getX(), box.getLowerBound().getX())
                 - std::min(_upperBound.getX(), box.getUpperBound().getX());
  const DBU dy = std::max(_lowerBound.getY(), box.getLowerBound().getY())
                 - std::min(_upperBound.getY(), box.getUpperBound().getY());

  return (dx < 0 && dy < 0);
}

bool Box::inside(Box box)
{
  return (box.getLowerBound().getX() >= _lowerBound.getX())
         && (box.getLowerBound().getX() <= _upperBound.getX())
         && (box.getLowerBound().getY() >= _lowerBound.getY())
         && (box.getLowerBound().getY() <= _upperBound.getY())
         && (box.getUpperBound().getX() >= _lowerBound.getX())
         && (box.getUpperBound().getX() <= _upperBound.getX())
         && (box.getUpperBound().getY() >= _lowerBound.getY())
         && (box.getUpperBound().getY() <= _upperBound.getY());
}

}  // namespace FastRoute