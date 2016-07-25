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
 * $Id: gmperiodgenerator.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file periodgenerator.h
 * @brief Generation of periods with constrained hyperperiod
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVAL_GMPERIODGENERATOR_H
#define MKEVAL_GMPERIODGENERATOR_H 1

#include <mkeval/periodgenerator.h>

#include <core/primitives.h>
#include <utils/random.h>

namespace tmssim {

  #define PTAB_ROWS 6
  #define PTAB_COLUMNS 10

  #define PERIODS_LOW 0
  #define PERIOD_HIGH 1
  
  /**
   * Implementation of period generation according to
   * Joel Goossens, Christophe Macq, Limitation of the Hyper-Period in
   * Real-Time Periodic Task Set Generation, Proceedings of the 9th
   * International Conference on Real-Time Systems (RTS'01)
   */
  class GmPeriodGenerator : public PeriodGenerator {
  public:
    enum Policy {
      LOWER = 0,
      LOW,
      HIGH
    };

    /**
     * @param _random deprecated, remove later
     */
    GmPeriodGenerator(Policy _policy/*, Random& _random*/);
    
    /**
     * Deprecated, should be removed
     */
    //TmsTimeInterval getNumber();
    TmsTimeInterval getNumber(Random& _random);
    TmsTimeInterval getNumber(unsigned int seed);
    
    
  private:
    //typedef TmsTimeInterval FactorTable[PTAB_ROWS][PTAB_COLUMNS];
    
    static const TmsTimeInterval lowerPeriods[PTAB_ROWS][PTAB_COLUMNS];
    static const TmsTimeInterval lowPeriods[PTAB_ROWS][PTAB_COLUMNS];
    static const TmsTimeInterval highPeriods[PTAB_ROWS][PTAB_COLUMNS];

    Policy policy;
    //Random& random;
    const TmsTimeInterval (* myTab)[PTAB_ROWS][PTAB_COLUMNS];
  };
  

} // NS tmssim

#endif // !MKEVAL_GMPERIODGENERATOR_H
