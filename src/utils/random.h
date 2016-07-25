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
 * $Id: random.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file random.h
 * @brief Random number generator.
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef UTILS_RANDOM_H
#define UTILS_RANDOM_H 1

#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <cstdint>

namespace tmssim {

  /**
   * This class provides a wrapper around the pseudo random number generator
   * of the libc. Additionally, it comprises some functions to increase
   * the usability of the random number generator.
   */
  class Random {
  public:
    /**
     * @brief Default constructor.
     * Constructs a new random object. If no seed is given, the current time
     * is used to initialise the seed.
     * @param seed Seed
     */
    Random(unsigned int seed = time(NULL));

    /**
     * @return The original seed of the generator
     */
    unsigned int getInitialSeed() const;

    /**
     * @return The current seed of the generator
     */
    unsigned int getCurrentSeed() const;

    /**
     * @brief Set a new seed for the generator
     * @param seed Seed
     */
    void setSeed(unsigned int seed);
    
    /**
     * @brief Wrapper around rand()/rand_r() from libc
     * @return a new pseud-random number
     */
    int getNumber();
    

    /**
     * @brief Generate a random number from a closed interval.
     * @param min Lower bound of the interval
     * @param max upper bound of the interval
     * @return a random number from [min,max] (bounds included!)
     */
    unsigned int getIntervalRand(unsigned int min, unsigned int max);

    /**
     * @brief Generate a 64 bit random number.
     * The implementation of this function assumes that RAND_MAX=0x7fffffff!
     * @return 64 random bits
     */
    uint64_t getNumber64();
    
  private:
    unsigned int initialSeed;
    unsigned int currentSeed;
  };

} // NS tmssim

#endif // !UTILS_RANDOM_H
