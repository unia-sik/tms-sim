/**
 * $Id: periodgenerator.cpp 875 2015-03-19 14:48:15Z klugeflo $
 * @file periodgenerator.cpp
 * @brief Generation of periods with constrained hyperperiod
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <tseval/periodgenerator.h>

#include <utils/tmsexception.h>

#include <iostream>
#include <string>

using namespace std;

namespace tmssim {

  const TmsTimeInterval PeriodGenerator::lowPeriods[PTAB_ROWS][PTAB_COLUMNS] = {
    {   1,   1,   2,   2,   4,   4,   8,   8,  16,  32 },
    {   1,   1,   3,   3,   3,   3,   9,   9,   9,  27 },
    {   1,   1,   1,   5,   5,   5,   5,   5,  25,  25 },
    {   7,   7,   7,   7,   7,   7,   7,   7,   7,  49 },
    {   1,   1,   1,   1,   1,   1,   1,  11,  11, 121 },
    {   1,   1,   1,   1,   1,   1,   1,  13,  13,  13 }
  };

  const TmsTimeInterval PeriodGenerator::highPeriods[PTAB_ROWS][PTAB_COLUMNS] = {
    {   1,   2,   4,   8,   8,  16,  16,  16,  32,  32 },
    {   1,   3,   3,   9,   9,   9,   9,  27,  27,  27 },
    {   1,   1,   5,   5,   5,   5,  25,  25,  25,  25 },
    {   1,   1,   7,   7,   7,   7,   7,  49,  49,  49 },
    {   1,   1,   1,  11,  11,  11,  11, 121, 121, 121 },
    {   1,   1,   1,   1,  13,  13,  13,  13,  13,  13 }
  };


  PeriodGenerator::PeriodGenerator(PeriodGenerator::Policy _policy, Random& _random)
    : policy(_policy), random(_random)
  {
    /*
    cout << "Low @ " << &lowPeriods << endl;
    cout << "High @ " << & highPeriods << endl;
    */
    switch (policy) {
    case PeriodGenerator::LOW:
      myTab = &PeriodGenerator::lowPeriods;
      break;
    case PeriodGenerator::HIGH:
      myTab = &PeriodGenerator::highPeriods;
      break;
    default:
      throw TMSException("Invalid policy");
    }
  }

  
  TmsTimeInterval PeriodGenerator::getNumber() {
    TmsTimeInterval period = 1;
    do {
      // TODO: adjust max for real eval
      for (size_t i = 0; i < PTAB_ROWS; ++i) { // -X only for testing
	size_t offset = random.getIntervalRand(0, PTAB_COLUMNS - 1);
	//cout << "(" << offset << ": " << (*myTab)[i][offset] << ") ";
	period *= (*myTab)[i][offset];
      }
      //cout << "- " << period << endl;
    } while (period <=2);
    return period;
  }


} // NS tmssim
