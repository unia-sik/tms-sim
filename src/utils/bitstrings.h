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
 * $Id: bitstrings.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file bitstrings.h
 * @brief Print bit strings
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef UTILS_BITSTRINGS_H
#define UTILS_BITSTRINGS_H

#include <iostream>
#include <cstdint>
#include <string>

namespace tmssim {

  /**
   * Print last length bits
   */
  void printBitString(std::ostream& ost, uint64_t bits, size_t length=64);
  
  /**
   * Print last length bits
   */
  void printBitString(std::ostream& ost, uint32_t bits, size_t length=32);

  /**
   * Print last length bits
   */
  std::string strBitString(uint64_t bits, size_t length=64);
  
  /**
   * Print last length bits
   */
  std::string strBitString(uint32_t bits, size_t length=32);
  
  
} // NS tmssim

#endif // !UTILS_BITSTRINGS_H
