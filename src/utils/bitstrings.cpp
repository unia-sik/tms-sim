/*
 * This file is part of tms-sim.
 *
 * Copyright 2014 University of Augsburg
 *
 * tms-sim is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tms-sim is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tms-sim.  If not, see <http://www.gnu.org/licenses/>.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the copyright holder.
 */

/**
 * $Id: bitstrings.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file bitstrings.cpp
 * @brief Print bit strings
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <utils/bitstrings.h>
#include <algorithm>
#include <sstream>

using namespace std;

namespace tmssim {

  
  
  void printBitString(ostream& ost, uint64_t bits, size_t length) {
    size_t l = min(length, (size_t)64);
    for (size_t o = 1; o <= l; ++o) {
      if (bits & (1LL << (l-o)))
	ost << "1";
      else
	ost << "0";
    }
  }

  
  void printBitString(ostream& ost, uint32_t bits, size_t length) {
    size_t l = min(length, (size_t)32);
    for (size_t o = 1; o <= l; ++o) {
      if (bits & (1 << (l-o)))
	ost << "1";
      else
	ost << "0";
    }
  }


  string strBitString(uint64_t bits, size_t length) {
    ostringstream oss;
    printBitString(oss, bits, length);
    return oss.str();
  }
  

  string strBitString(uint32_t bits, size_t length) {
    ostringstream oss;
    printBitString(oss, bits, length);
    return oss.str();
  }

} // NS tmssim
