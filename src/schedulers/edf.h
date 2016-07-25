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
 * $Id: edf.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file edf.h
 * @brief EDF Scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_EDF_H
#define SCHEDULERS_EDF_H 1

#include <schedulers/ald.h>

namespace tmssim {

  /**
   * @brief Earliest deadline first scheduler
   * @todo implement deadline monitoring, cancel job if deadline miss
   * is pending.
   */
  class EDFScheduler : public ALDScheduler {
    
  public:
    EDFScheduler(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);
    virtual ~EDFScheduler();

    virtual void enqueueJob(Job *job);

      
    /**
     * This function checks whether the first job in the queue, i.e. the
     * job that is executed next, can still keep its deadline.
     * If a deadline miss is pending, the job is immediately cancelled.
     * You may overwrite it in subclasses to implement more sophisticated
     * cancellation policies.
     */
    virtual int schedule(TmsTime now, ScheduleStat& scheduleStat);


    virtual const std::string& getId(void) const;

    
    //friend std::ostream& operator << (std::ostream& ost, const EDFScheduler& scheduler);
    
    static const Job* checkEDFSchedule(TmsTime now,
				       const list<Job*>& schedule);
    
  protected:
    /*
    virtual int doSchedule(int now, ScheduleStat& scheduleStat);
    virtual Job* doDispatch(int now, DispatchStat& dispatchStat);
    virtual bool fireCancelJob(Job *job);
    */

    /**
      Create a EDF schedule for the current job list
    */
    //void createEDFSchedule(void);

    /**
     * @brief Check a EDF schedule for feasibility.
     * Runs through EDFScheduler::myScheduled an checks whether all jobs can
     * keep their deadlines if the schedule is started at a certain time.
     * @param now current time
     * @return the first job that would miss its deadline, NULL else
     */
    const Job* checkEDFSchedule(TmsTime now) const;

  };


  /**
   * @brief Generic allocator function
   */
  Scheduler* EDFSchedulerAllocator(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);


  //std::ostream& operator << (std::ostream& ost, const EDFScheduler& scheduler);

} // NS tmssim

#endif /* SCHEDULERS_EDF_H */

