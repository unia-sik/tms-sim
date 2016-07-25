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
 * $Id: scheduler.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file scheduler.h
 * @brief Definition of the abstract scheduler class
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef CORE_SCHEDULER_H
#define CORE_SCHEDULER_H 1

#include <string>
#include <vector>

#include <core/scobjects.h>
//#include <utils/logger.h>
#include <core/scconfig.h>

namespace tmssim {

  /// Use this macro to check Job* return values
#define SC_ERR(e) ((Job *)(e))
  

  /**
   * Scheduling statistics.
   * @todo make list contain const Job*
   */
  struct ScheduleStat {
    ScheduleStat();
    ~ScheduleStat();
    
    std::list<Job*> cancelled;
  };
  
  
  /**
    Dispatch statistics
  */
  struct DispatchStat {
    DispatchStat();
    
    const Job* executed;
    const Job* finished;
    bool dlMiss;
    bool idle;
  };
  

  /**
   * @brief Interface for the implementation of Schedulers
   * Inherit from from this class to implement your own schedulers
   */
  class Scheduler {
  public: // errors
    // Schedule errors
    static const int ESC_SCED_SRCH = -1; ///< No job for removal found
    // Dispatch errors
    static const int ESC_DISP_TIME = -1; ///< Time step too big
    static const int ESC_DISP_COMP = -2; ///< Job completion failed
    static const int ESC_DISP_LIST = -3; ///< Job list changed since last schedule call
    static const int ESC_DISP_STAT = -4; ///< Invalid dispatch statistics

  public:
    //Scheduler(/*Logger* _logger = noLogPtr*/);// : logger(_logger) {}

    /**
     * @brief Virtual destructor for subclassing
     * Make sure in your implementation to check whether the scheduler
     * still contains pending jobs. If so, take care to clean up
     * their memory correctly!
     */
    virtual ~Scheduler() = 0;

    /**
     * @brief Enqueue a new job for execution
     *
     * Any job that is enqueued must be returned to the simulation later
     * either by being cancelled via the scheduleStat parameter of
     * the #schedule method, or by finishing execution as a return
     * value of the #dispatch method.
     * @param job a new Job
     * @return this method should always return true, i.e. enqueuing a job
     * should never fail. If enqueuing of a job leads to an invalid schedule,
     * this should be notified by the #schedule or #dispatch methods.
     * @todo remove return value
     */
    virtual void enqueueJob(Job *job) = 0;

    /**
     * @brief Remove an active job that was previously enqueued
     * @return A pointer to the removed job, or NULL if no job was found
     */
    virtual const Job* removeJob(const Job *job) = 0;

    /**
     * @brief Init scheduler for new time step.
     *
     * This method should perform cancellations of jobs that no longer can
     * keep their deadlines. This is important if cancellation of a job and
     * activation of its successor occur in the same time step. Cancellation
     * may alter that task's state, which might influence that new job.
     * In the old implementation, all cancellations are performed after the
     * activations, and thus some jobs' states might not be correct.
     * @param now Point in time, succeeding calls to this functions must have
     * increasing times
     * @param[out] dispatchStat Container for cancelled jobs.
     * @return 0
     */
    virtual int initStep(TmsTime now, ScheduleStat& scheduleStat) = 0;

    
    /**
     * @brief Create the schedule
     * Implementations may decide to leave this function empty and
     * perform the relevant work already during the enqueueJob method.
     * @param now Point in time, succeeding calls to this functions must have
     * increasing times
     * @param[out] scheduleStat Container for statistics. If the scheduler
     * decides to cancel some jobs, they will be stored in this container.
     * @return 0 on success, some error code else
     */
    virtual int schedule(TmsTime now, ScheduleStat& scheduleStat) = 0;
    
    /**
     * Dispatch one job from the schedule for execution
     * @param now Point in time, succeeding calls to this functions must have
     * increasing times
     * @param[out] dispatchStat Container for statistics. If a job finished
     * execution, it is also written to the dispatchStat struct.
     * @return finished job, NULL (running/idling), or ESC_DISP error code
     */
    virtual Job* dispatch(TmsTime now, DispatchStat& dispatchStat) = 0;
    
    /**
     * @brief Check if there are enqueued jobs
     */
    virtual bool hasPendingJobs(void) const = 0;
    
    /**
     * @brief Get some readable ID from a scheduler
     */
    virtual const std::string& getId(void) const = 0;

    /**
     * @brief output the current schedule.
     * This method is mostly for debugging purposes.
     */
    virtual void printSchedule() const {}

  protected:
    /**
     * @brief Logging/tracing of execution
     */
    //Logger* logger;
    
  };

  inline Scheduler::~Scheduler() { }
  
} // NS tmssim

#endif /* CORE_SCHEDULER_H */
