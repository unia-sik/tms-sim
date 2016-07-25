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
 * $Id: gmperiodgenerator.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file periodgenerator.cpp
 * @brief Generation of periods with constrained hyperperiod
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <mkeval/gmperiodgenerator.h>

#include <utils/tmsexception.h>

#include <iostream>
#include <string>

using namespace std;

namespace tmssim {

    const TmsTimeInterval GmPeriodGenerator::lowerPeriods[PTAB_ROWS][PTAB_COLUMNS] = {
    {   1,   1,   2,   2,   4,   4,   8,   8,  16,  32 },
    {   1,   1,   3,   3,   3,   3,   9,   9,   9,   9 },
    {   1,   1,   1,   5,   5,   5,   5,   5,  25,  25 },
    {   7,   7,   7,   7,   7,   7,   7,   7,   7,  49 },
    {   1,   1,   1,   1,   1,   1,   1,  11,  11,  11 },
    {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 }
  };

  const TmsTimeInterval GmPeriodGenerator::lowPeriods[PTAB_ROWS][PTAB_COLUMNS] = {
    {   1,   1,   2,   2,   4,   4,   8,   8,  16,  32 },
    {   1,   1,   3,   3,   3,   3,   9,   9,   9,  27 },
    {   1,   1,   1,   5,   5,   5,   5,   5,  25,  25 },
    {   7,   7,   7,   7,   7,   7,   7,   7,   7,  49 },
    {   1,   1,   1,   1,   1,   1,   1,  11,  11, 121 },
    {   1,   1,   1,   1,   1,   1,   1,  13,  13,  13 }
  };

  const TmsTimeInterval GmPeriodGenerator::highPeriods[PTAB_ROWS][PTAB_COLUMNS] = {
    {   1,   2,   4,   8,   8,  16,  16,  16,  32,  32 },
    {   1,   3,   3,   9,   9,   9,   9,  27,  27,  27 },
    {   1,   1,   5,   5,   5,   5,  25,  25,  25,  25 },
    {   1,   1,   7,   7,   7,   7,   7,  49,  49,  49 },
    {   1,   1,   1,  11,  11,  11,  11, 121, 121, 121 },
    {   1,   1,   1,   1,  13,  13,  13,  13,  13,  13 }
  };


  GmPeriodGenerator::GmPeriodGenerator(GmPeriodGenerator::Policy _policy/*, Random& _random*/)
    : policy(_policy)//, random(_random)
  {
    /*
    cout << "Low @ " << &lowPeriods << endl;
    cout << "High @ " << & highPeriods << endl;
    */
    switch (policy) {
    case GmPeriodGenerator::LOWER:
      myTab = &GmPeriodGenerator::lowerPeriods;
      break;
    case GmPeriodGenerator::LOW:
      myTab = &GmPeriodGenerator::lowPeriods;
      break;
    case GmPeriodGenerator::HIGH:
      myTab = &GmPeriodGenerator::highPeriods;
      break;
    default:
      throw TMSException("Invalid policy");
    }
  }

  
  TmsTimeInterval GmPeriodGenerator::getNumber(Random& _random) {
    TmsTimeInterval period = 1;
    do {
      // TODO: adjust max for real eval
      for (size_t i = 0; i < PTAB_ROWS; ++i) { // -X only for testing
	size_t offset = _random.getIntervalRand(0, PTAB_COLUMNS - 1);
	//cout << "(" << offset << ": " << (*myTab)[i][offset] << ") ";
	period *= (*myTab)[i][offset];
      }
      //cout << "- " << period << endl;
    } while (period <=2);
    return period;
  }

  /*
  TmsTimeInterval GmPeriodGenerator::getNumber() {
    return getNumber(random);
  }
  */

  TmsTimeInterval GmPeriodGenerator::getNumber(unsigned int seed) {
    Random _random(seed);
    return getNumber(_random);
  }


} // NS tmssim
