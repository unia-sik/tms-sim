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
 * $Id: random.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file random.cpp
 * @brief Random number generator.
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <utils/random.h>

#include <cassert>

namespace tmssim {

  Random::Random(unsigned int seed)
    : initialSeed(seed), currentSeed(seed) {
  }


  unsigned int Random::getInitialSeed() const {
    return initialSeed;
  }

  
  unsigned int Random::getCurrentSeed() const {
    return currentSeed;
  }

  
  void Random::setSeed(unsigned int seed) {
    initialSeed = seed;
    currentSeed = seed;
  }

  
  int Random::getNumber() {
    return rand_r(&currentSeed);
  }

  
  unsigned int Random::getIntervalRand(unsigned int min, unsigned int max) {
      assert(min < max);
      unsigned int interval = max - min + 1;
      assert(interval <= RAND_MAX);
      unsigned int val = rand_r(&currentSeed);
      unsigned int result = val % interval + min;
      assert((min <= result) && (result <= max));
      return result;
  }

  
  uint64_t Random::getNumber64() {
    uint64_t number = 0LL;
    uint64_t lo = rand_r(&currentSeed);
    uint64_t hi = rand_r(&currentSeed);
    uint32_t msbits = rand_r(&currentSeed);
    if (msbits & 0x1) {
      lo |= 1LL << 31;
    }
    if (msbits & 0x2) {
      hi |= 1LL << 31;
    }
    number = (hi << 32) | lo;
    return number;
  }

} // NS tmssim
