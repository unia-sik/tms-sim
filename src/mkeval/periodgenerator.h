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
 * $Id: periodgenerator.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file periodgenerator.h
 * @brief Abstract class for period generation
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVAL_PERIODGENERATOR_H
#define MKEVAL_PERIODGENERATOR_H 1

#include <core/primitives.h>
#include <utils/random.h>

namespace tmssim {

  class PeriodGenerator {
  public:
    PeriodGenerator();
    virtual ~PeriodGenerator();

    virtual TmsTimeInterval getNumber(Random& random) = 0;
    virtual TmsTimeInterval getNumber(unsigned int seed) = 0;
    
  };
  

} // NS tmssim

#endif // !MKEVAL_PERIODGENERATOR_H
