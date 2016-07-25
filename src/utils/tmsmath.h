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
 */

/**
 * $Id: tmsmath.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file tmsmath.h
 * @brief Common math functions
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef UTILS_TMSMATH_H
#define UTILS_TMSMATH_H

#include <core/primitives.h>

namespace tmssim {
  
  /**
   * @brief Least Common multiple.
   * Uses 
   * \operatorname{gcd}(m,n) \cdot \operatorname{lcm}(m,n) = |m \cdot n| 
   * @param a
   * @param b
   * @return LCM
   */
  TmsTime calculateLcm(TmsTime a, TmsTime b);
  
  /**
   * @brief Greatest common divisor.
   * Uses Euclid algorithm:
   * function gcd(a, b)
   *   while b ≠ 0
   *     t := b
   *     b := a mod b
   *     a := t
   *   return a
   * @param a
   * @param b
   * @return GCD
   */
  TmsTime calculateGcd(TmsTime a, TmsTime b);


  /**
   * calculate binomial coefficient n over k
   */
  unsigned long long int binomialCoefficient(unsigned n, unsigned k);
  
} // NS tmssim

#endif // !UTILS_TMSMATH_H
