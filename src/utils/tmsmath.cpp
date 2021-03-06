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
 * $Id: tmsmath.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file tmsmath.cpp
 * @brief Common math functions
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <utils/tmsmath.h>

#include <algorithm>
#include <cstdlib>

using namespace std;

namespace tmssim {
  
  TmsTime calculateLcm(TmsTime a, TmsTime b) {
    return llabs(a * b) / calculateGcd(a, b);
  }
  
  
  TmsTime calculateGcd(TmsTime a, TmsTime b) {
    TmsTime h;
    while (b != 0) {
      h = b;
      b = a % b;
      a = h;
    }
    return a;
  }
  

  // implementation is based on code from
  // http://www.geeksforgeeks.org/dynamic-programming-set-9-binomial-coefficient/
  unsigned long long int binomialCoefficient(unsigned n, unsigned k) {
    unsigned long long int C[k+1];
    size_t c = 0;
    for (c = 0; c < k+1; ++c)
      C[c] = 0;
    unsigned int i;
    int j; // cannot be unsigned!
    
    C[0] = 1;
    for(i = 1; i <= n; i++) {
      for(j = min(i, k); j > 0; j--)
	C[j] = C[j] + C[j-1];
    }

    return C[k];
  }
  
} // NS tmssim
