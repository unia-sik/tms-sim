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
 * $Id: gdpa.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file gdpa.h
 * @brief Guaranteed Dynamic Priority Assigment Scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULER_GDPA_H
#define SCHEDULER_GDPA_H 1

#include <schedulers/edf.h>


namespace tmssim {

  /**
   * @brief GDPA Scheduler.
   *
   * This class implements GDPA from Cho et al., "Guaranteed Dynamic
   * Priority Assignment Schemes", ETRI Journal Vol. 32 No. 3, June 2010
   */
  class GDPAScheduler : public EDFScheduler {
  public:
    GDPAScheduler(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);
    virtual ~GDPAScheduler();

    virtual void enqueueJob(Job *job);

    virtual bool hasPendingJobs(void) const;

    //virtual int initStep(TmsTime now, ScheduleStat& scheduleStat);

    
    /**
     * @brief This function calculates the actual GDPA schedule.
     *
     * This method implements algorithm 1 from Cho et al. 2010
     */
    virtual int schedule(TmsTime now, ScheduleStat& scheduleStat);


    virtual const std::string& getId(void) const;

  protected:
    virtual void jobFinished(Job *job);


  private:
    std::list<Job*> readyQueue;
    bool readyQueueChanged;
  };


  /**
   * @brief Generic allocator function
   */
  Scheduler* GDPASchedulerAllocator(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);

} // NS tmssim

#endif // !SCHEDULER_GDPA_H
