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
 * $Id: fpp.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file fpp.h
 * @brief Fixed Priority Preemptive Scheduler
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

#ifndef SCHEDULERS_FPP_H
#define SCHEDULERS_FPP_H 1

namespace tmssim {

  /**
   * @brief Fixed Priority Preemptive Scheduling
   */
  class FPPScheduler : public ALDScheduler {
  public:
    static const unsigned int MAX_PRIORITY = 0;
    static const unsigned int MIN_PRIORITY = UINT_MAX;

  public:
    FPPScheduler(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);
    virtual ~FPPScheduler();

    //virtual int initStep(TmsTime now, ScheduleStat& scheduleStat);
    
    /**
     * @brief Enqueue a job for priority-ordered execution.
     * Jobs are executed in ascending order of their priority values.
     * FPPScheduler::MAX_PRIORITY (0) represents the most important
     * jobs, jobs with priority FPPScheduler::MIN_PRIORITY (UINT_MAX)
     * are executed last.
     */
    virtual void enqueueJob(Job *job);

    /**
     * This function does nothing. You may overwrite it in subclasses
     * to e.g. check whether a schedule is feasible and, if not, make
     * it feasible.
     */
    virtual int schedule(TmsTime now, ScheduleStat& scheduleStat);

    //virtual Job* dispatch(TmsTime now, DispatchStat& dispatchStat);


    virtual const std::string& getId(void) const;


    
    
  protected:
    /*
     * @brief Check a FPP schedule for feasibility.
     * Runs through FPPScheduler::mySchedule an checks whether all jobs can
     * keep their deadlines if the schedule is started at a certain time.
     * @param now current time
     * @return the first job that would miss its deadline, NULL else
     */
    //const Job* checkFPPSchedule(int now) const;
    //DeadlineMonitor dlmon;

    /**
     * Jobs that miss their deadline but must still be executed.
     */
    //std::list<Job*> execMissJobs;
    
  }; // class FPPScheduler


  /**
   * @brief Generic allocator function
   */
  Scheduler* FPPSchedulerAllocator(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);

} // NS tmssim

#endif /* !SCHEDULERS_FPP_H */
