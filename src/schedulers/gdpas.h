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
 * $Id: gdpas.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file gdpas.h
 * @brief Simplified Guaranteed Dynamic Priority Assigment Scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_GDPAS_H
#define SCHEDULERS_GDPAS_H 1

#include <core/scheduler.h>

namespace tmssim {

  /**
   * @brief GDPA-S Scheduler.
   *
   * This class implements GDPA-S from Cho et al., "Guaranteed Dynamic
   * Priority Assignment Schemes", ETRI Journal Vol. 32 No. 3, June 2010
   */
  class GDPASScheduler : public Scheduler {
  public:
    GDPASScheduler(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);
    virtual ~GDPASScheduler();

    virtual void enqueueJob(Job *job);
    virtual const Job* removeJob(const Job *job);

    virtual bool hasPendingJobs(void) const;

    //virtual int initStep(__attribute__((unused)) TmsTime now,
    //__attribute__((unused)) ScheduleStat& scheduleStat) { return 0;
    //};
    virtual int initStep(TmsTime now, ScheduleStat& scheduleStat);

    /**
     * @brief This function calculates the actual GDPA-S schedule.
     *
     * This method implements algorithm 2 from Cho et al. 2010
     */
    virtual int schedule(TmsTime now, ScheduleStat& scheduleStat);

    virtual Job* dispatch(TmsTime now, DispatchStat& dispatchStat);

    virtual const std::string& getId(void) const;

  protected:
    virtual void jobFinished(Job *job);

    /**
     * @brief Configuration data for execution behaviour.
     */
    SchedulerConfiguration myConfig;

  private:
    void feasibilityCheck(TmsTime now);
    void removeJobFromList(std::list<Job*>& jList, Job* job);
    std::list<Job*> readyList;
    std::list<Job*> sdfList;
    std::list<Job*> edfList;
    //bool readyListChanged;
    bool dispatchListsChanged;
    //bool queuesChanged;
    //bool edfFeasible;
    Job* currentJob;
  };


  /**
   * @brief Generic allocator function
   */
  Scheduler* GDPASSchedulerAllocator(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);

} // NS tmssim

#endif // !SCHEDULERS_GDPAS_H
