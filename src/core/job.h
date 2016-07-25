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
 * $Id: job.h 1407 2016-05-31 07:30:51Z klugeflo $
 * @file job.h
 * @brief Definition of Job class
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef CORE_JOB_H
#define CORE_JOB_H 1

#include <string>

#include <core/primitives.h>

namespace tmssim {

  class Task;

  /**
    @brief Representation of a job that must be executed.
    Jobs represent the units of work that are executed by a Scheduler.
  */
  class Job {
    
  public:
    /**
      @param task owner task
      @param jid job id (usually job number)
      @param _activationTime activation time
      @param _executionTime execution time span
      @param _absDeadline absolute Deadline
      @param _priority static priority (only for static priority scheduling)
    */
    Job(Task* task, unsigned int jid, TmsTime _activationTime,
	TmsTimeInterval _executionTime, TmsTime _absDeadline,
	TmsPriority _priority);
    //Job(const Job& rhs);
    virtual ~Job();
    
    TmsTime getActivationTime(void) const; ///< get activation time
    TmsTimeInterval getExecutionTime(void) const; ///< get execution time
    TmsTime getAbsDeadline(void) const; ///< get critial time
    TmsTimeInterval getRemainingExecutionTime(void) const; ///< get remaining execution time
    Task* getTask(void) const; ///< get owner task
    TmsPriority getPriority(); ///< get (static) priority
    TmsTime getLatestStartTime(void) const; ///< get latest start time

    bool isFeasible(TmsTime now) const;
    
    void updateLatestStartTime(void);
    
    bool execStep(TmsTime now); ///< execute one step; @return true if remaining ET is zero
    void preempt(); ///< notification from scheduler - job is preempted
    void setPriority(TmsPriority p); ///< set static priority
    int getPreemptions() const; ///< get number of  preemptions
    
    /**
      @brief calculate possible benefit if execution start at startTime
      @param startTime
      @return utility that can be achieved
      @see #tmssim::Task::getPossibleExecValue
    */
    double getPossibleExecValue(TmsTime startTime) const;
    
    /**
      @brief calculate possible history-cognisant utility of task if execution
      start at startTime.
      @param startTime
      @return HCU the task can achieve
      @see #tmssim::Task::getPossibleHistoryValue
    */
    double getPossibleHistoryValue(TmsTime startTime) const;

    /**
     * @brief calculate possible history-cognisant utility of task
     * if job is cancelled
     * @return HCU the task can achieve
     * @see #tmssim::Task::getPossibleHistoryValue
     */
    double getPossibleFailHistoryValue() const;
    
    std::string getIdString(void) const; ///< get job identifier for verbose output
    
    unsigned int getJobId(void) const { return jobId; }

  protected:
    Task* myTask; ///< owner task
    unsigned int jobId; ///< usually job number
    TmsTime activationTime; ///< arrival time
    TmsTimeInterval executionTime; ///< execution time
    TmsTime absDeadline; ///< critical time
    TmsPriority priority; ///< priority
    TmsTimeInterval etRemain; ///< remaining execution time
    int preemptions; ///< count dispatch/schedule preemptions
    TmsTime latestStartTime; ///< latest start time
    
  public:
    friend std::ostream& operator << (std::ostream& ost, const Job& job);
  };


} // NS tmssim

#endif // !CORE_JOB_H
