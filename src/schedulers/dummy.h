/**
 * $Id: dummy.h 1421 2016-06-22 07:46:32Z klugeflo $
 * @file dummy.h
 * @brief Dummy scheduler that does nothing
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_DUMMY_H
#define SCHEDULERS_DUMMY_H 1

#include <core/scheduler.h>

#include <utils/tmsexception.h>

namespace tmssim {

  class DummyScheduler : public Scheduler {

  public:

    DummyScheduler() {}
    ~DummyScheduler() {}

    void enqueueJob(__attribute__((unused)) Job *job) {
      throw TMSException("Cannot enqueue job into DummyScheduler!");
    }
    
    const Job* removeJob(__attribute__((unused)) const Job *job) { return NULL; }
    int initStep(__attribute__((unused)) TmsTime now, __attribute__((unused)) ScheduleStat& scheduleStat) { return 0; }

    int schedule(__attribute__((unused)) TmsTime now, __attribute__((unused)) ScheduleStat& scheduleStat) { return 0; }

    Job* dispatch(__attribute__((unused)) TmsTime now, __attribute__((unused)) DispatchStat& dispatchStat) { return 0; }
    
    bool hasPendingJobs(void) const { return false; }
    
    const std::string& getId(void) const;

  };

} // NS tmssim

#endif // !SCHEDULERS_DUMMY_H
