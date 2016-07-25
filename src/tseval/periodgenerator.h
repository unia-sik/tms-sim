/**
 * $Id: periodgenerator.h 860 2015-01-22 16:27:32Z klugeflo $
 * @file periodgenerator.h
 * @brief Generation of periods with constrained hyperperiod
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef TSEVAL_PERIODGENERATOR_H
#define TSEVAL_PERIODGENERATOR_H 1

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
  class PeriodGenerator {
  public:
    enum Policy {
      LOW = 0,
      HIGH = 1
    };
    
    PeriodGenerator(Policy _policy, Random& _random);
    TmsTimeInterval getNumber();
    
    
  private:
    //typedef TmsTimeInterval FactorTable[PTAB_ROWS][PTAB_COLUMNS];
    
    static const TmsTimeInterval lowPeriods[PTAB_ROWS][PTAB_COLUMNS];
    static const TmsTimeInterval highPeriods[PTAB_ROWS][PTAB_COLUMNS];

    Policy policy;
    Random& random;
    const TmsTimeInterval (* myTab)[PTAB_ROWS][PTAB_COLUMNS];
  };
  

} // NS tmssim

#endif // !TSEVAL_PERIODGENERATOR_H
