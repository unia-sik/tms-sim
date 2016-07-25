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
 * $Id: fppnat.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file fppnat.h
 * @brief Fixed Priority Preemptive Scheduler with "natural" priority ordering
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 * @todo adjust stdint.h include to cstdint as soon as C++ 2011 is stable
 */

#include <schedulers/ald.h>
#include <core/deadlinemonitor.h>

//#include <cstdint>
/**
 */
#include <stdint.h>
#include <limits.h>

#ifndef SCHEDULERS_FPPNAT_H
#define SCHEDULERS_FPPNAT_H 1

namespace tmssim {

  /**
   * @brief Fixed Priority Preemptive Scheduling
   * @todo implement deadline monitoring, cancel job if deadline miss
   * is pending.
   */
  class FPPNatScheduler : public ALDScheduler {
  public:
    static const unsigned int MAX_PRIORITY = 0;
    static const unsigned int MIN_PRIORITY = UINT_MAX;

  public:
    FPPNatScheduler();
    virtual ~FPPNatScheduler();
    
    /**
     * @brief Enqueue a job for priority-ordered execution.
     * Jobs are executed in ascending order of their priority values.
     * FPPNatScheduler::MAX_PRIORITY (0) represents the most important
     * jobs, jobs with priority FPPNatScheduler::MIN_PRIORITY (UINT_MAX)
     * are executed last.
     */
    virtual void enqueueJob(Job *job);

    /**
     * This function does nothing. You may overwrite it in subclasses
     * to e.g. check whether a schedule is feasible and, if not, make
     * it feasible.
     */
    virtual int schedule(TmsTime now, ScheduleStat& scheduleStat);

    virtual Job* dispatch(TmsTime now, DispatchStat& dispatchStat);


    virtual const std::string& getId(void) const;


    
    
  protected:
    /*
     * @brief Check a FPP schedule for feasibility.
     * Runs through FPPNatScheduler::mySchedule an checks whether all jobs can
     * keep their deadlines if the schedule is started at a certain time.
     * @param now current time
     * @return the first job that would miss its deadline, NULL else
     */
    //const Job* checkFPPSchedule(int now) const;
    DeadlineMonitor dlmon;

  }; // class FPPNatScheduler


  /**
   * @brief Generic allocator function
   */
  Scheduler* FPPNatSchedulerAllocator();

} // NS tmssim

#endif /* !SCHEDULERS_FPPNAT_H */
