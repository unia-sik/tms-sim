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
 * $Id: intervalperiodgenerator.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file intervalperiodgenerator.h
 * @brief Generate periods from an interval
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVAL_INTERVALPERIODGENERATOR_H
#define MKEVAL_INTERVALPERIODGENERATOR_H

#include <mkeval/periodgenerator.h>

namespace tmssim {

  class IntervalPeriodGenerator : public PeriodGenerator {
  public:
    IntervalPeriodGenerator(TmsTimeInterval _pMin, TmsTimeInterval _pMax);

    TmsTimeInterval getNumber(Random& random);

    /**
     * @param seed is directly used to calculate period. This is to keep
     * backward "compatibility" (create same task sets as without IPG)
     */
    TmsTimeInterval getNumber(unsigned int seed);


  private:
    TmsTimeInterval pMin;
    TmsTimeInterval pMax;
    
  };

} // NS tmssim

#endif // !MKEVAL_INTERVALPERIODGENERATOR_H
