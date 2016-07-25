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
 * $Id: task.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file task.h
 * @brief Definition of Task superclass
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef CORE_TASK_H
#define CORE_TASK_H 1

#include <core/primitives.h>

#include <core/writeabletoxml.h>
//#include <core/iwriteabletoxml.h>

#include <core/utilitycalculator.h>
#include <core/utilityaggregator.h>

#include <vector>

namespace tmssim {

  /**
   * @class Task
   * @brief Abstract task provides a general interface that is used for scheduling
   * 
   * A task generates jobs that must be executed by a #tmssim::Scheduler.
   * Use this class as a base for your actual implementation of task models.
   * Implement the abstract protected methods to with the task model behaviour.
   * 
   * @todo - currently, UtilityCalculator and UtilityAggregator are passed as
   * pointers. They must be allocated using new, the Task::~Task will take
   * care of freeing the memory. This is, however, very dirty, but works for
   * now.
   * We need to find some other way to manage utility generation later, e.g.
   * by using templates (which will be tricky too). However, this can also be
   * a problem if we have special types of tasks that need rather special
   * utility management.
   * - move statistics into separate object
   */
  class Task
    : //public IWriteableToXML,
    public WriteableToXML,
    public ICloneable<Task>
  {
    public:
    /**
     * 
     * Constructor can be called by concrete subclasses
     * @param __id an identifier for the task (which has no internal meaning)
     * @param __executionTime execution time of the task
     * @param __relDeadline critical time of the task relative to activation
     * @param __uc pass a pointer to a newly allocated UtilityCalculator.
     * The task will take care for freeing __uc's memory when it is destroyed.
     * @param __ua pass a pointer to a newly allocated UtilityAggregator.
     * The task will take care for freeing __uc's memory when it is destroyed.
     * @param __priority static priority of the task (for static priority
     * scheduling)     
     * @todo Should we move ET somewhere else for tasks that have
     * variable execution times? For the moment we won't do this, because the
     * utility functions need at least ET and CT
     */
    Task(unsigned int __id, TmsTime __executionTime,
	 TmsTimeInterval __relDeadline, UtilityCalculator* __uc,
	 UtilityAggregator* __ua, TmsPriority __priority);
    
    /// D'tor
    virtual ~Task(void);

    /// Copy Constructor
    Task(const Task& rhs);

    // this one is already contained in ICloneable
    //virtual Task* clone() const = 0;
    
    /// @name Task properties
    ///@{

    /// @return the identifier that was assigned to the task
    unsigned int getId(void) const;

    /// @return static priority
    TmsPriority getPriority(void) const;

    /**
     * @brief Set static priority.
     *
     * This function is made public so that in future versions of
     * tms-sim various static priority scheduling algorithms can
     * be added (e.g. RMA, DMA).
     * @param _priority the task's new prioriy
     */
    void setPriority(TmsPriority _priority);

    /// @return the execution time of the task
    TmsTimeInterval getExecutionTime(void) const;

    /// @return the critical time (=deadline) of the task
    TmsTimeInterval getRelativeDeadline(void) const;

    /**
     * @return the task's utility calculator
     */
    const UtilityCalculator* getUC() const;

    /**
     * @return the task's utility aggregator
     */
    const UtilityAggregator* getUA() const;

    /**
     * @brief Get the distance (number of failed jobs) to a failure state.
     *
     * Certain task types can tolerate that some job executions miss
     * their deadlines resp. are cancelled. However, this must not happen
     * too often. The implementation provided in this class is a default
     * implemenation that does not allow any deadline misses (i.e. it
     * always will return 1). Override this method if (1) your task model
     * implements a different deadline miss policy and (2) your scheduler
     * is actually aware of the failure-state distance.
     *
     * @return distance to invalid state (too many deadline misses).
     */
    int getDistance() const;
    ///@}    

    /// @name Task execution control
    ///@{
    /**
      Prepare the task for execution.
      This function should be called any time an execution trace is generated
      freshly. It will call the #tmssim::Task::startHook, where user
      implementations can (re)initialise their data.
      @param now starting time of simulation, usually 0
    */
    void start(TmsTime now=0);

    /**
      Checks whether the task can spawn a new job.
      This method will call the #tmssim::Task::spawnHook implemented by the
      concrete task class.
      @param now current simulated time
      @return a new job to execute, if one is available, NULL else
    */
    Job* spawnJob(TmsTime now);

    /**
      Notify a task that one of its jobs has completed.
      This method is called any by the simulation environment any time a job
      is completed. It will call the #tmssim::Task::completionHook of the user
      implementation.
      @param job the completed job
      @param now completion time
    */
    void completeJob(Job* job, TmsTime now);

    /**
     * Notify a task that one of its jobs has been cancelled.
     * This method is called by the simulation environment if the scheduler
     * decided to cancel a job. It will call the #tmssim::Task::cancelHook
     * of the user implementation
     * @param job the cancelled job
     * @return the return value of #tmssim::Task::cancelHook
     * @todo add time stamp?
     */
    bool cancelJob(Job* job);
    ///@}


    /// @name Task execution statistics
    ///@{
    /**
      @return the number of activations of this task
      @todo move to statistics class
    */
    int getActivations(void) const;

    /**
      @return the number the task was completed (including deadline misses)
      @todo move to statistics class
    */
    int getCompletions(void) const;

    /**
      @return the number of task instances (jobs) that were cancelled
      @todo move to statistics class
    */
    int getCancellations(void) const;

    /**
      @return the number of deadline misses
      @todo move to statistics class
    */
    int getMisses(void) const;

    /**
      @return the number of preemptions any job of this task incurred
      @todo move to statistics class
    */
    int getPreemptions(void) const;

    /**
      @return number of cancellation after job started execution
      @todo move to statistics class
    */
    int getExecCancellations(void) const;

    /**
     * @return Number of execution cycles lost due to exec cancellations
     */
    TmsTime getEcPerformanceLost(void) const;
    
    /**
      @return the counters of successive cancellations
     */
    const std::vector<int>& getCounters(void) const;
    ///@}

    /// @name Task and job utilities
    ///@{
    /**
      @return Utility of last job execution
    */
    virtual double getLastExecValue(void) const;

    /**
      @return aggregated utility
    */
    virtual double getHistoryValue(void) const;

    /**
      Check how much utility a Job of this task might yield if it is started
      at a certain time.
      @param job the Job to examine
      @param startTime when the job can be started
      @return the utility of the job, if started at startTime
    */
    virtual double getPossibleExecValue(const Job *job, TmsTime startTime) const;

    /**
      Check which history-cognisant utility the task would gain if a job is
      started at a certain time.
      @param job the Job to examine
      @param startTime when the job can be started
      @return the history-cognisant utility of the task, if the job is started
      at startTime
    */
    virtual double getPossibleHistoryValue(const Job *job, TmsTime startTime) const;

    /**
      Check which history-cognisant utility the task would gain if a job is
      cancelled
      @param job the Job to examine
      @return the history-cognisant utility of the task, if the job is cancelled
    */
    virtual double getPossibleFailHistoryValue(const Job *job) const;

    ///@}


    /// @name Verbosity
    ///@{
    /**
      @return string representation of the task
    */
    std::string getIdString(void) const;

    /**
      Output of a human-readable description of this task
    */
    virtual std::ostream& print(std::ostream& ost) const;

    //friend std::ostream& operator << (std::ostream& ost, const Task& task);

    virtual std::string strState() const;
    
    ///@}
    

    /**
     * @name XML
     * @{
     */

    /**
     * @todo better leave this out, as we don't want tasks to be written
     * to XML. Do we?
     */
    virtual const std::string& getClassElement();
    virtual int writeData(xmlTextWriterPtr writer);
    /**
     * @}
     */

  protected:

    // TASK EXECUTION HOOKS
    /// @name Task execution control
    ///@{
    /**
     * @return first activation time
     */
    virtual TmsTime startHook(TmsTime now) = 0;
    
    /**
     * This method is called by #tmssim::Task::spawnJob to create a new job.
     * Use #tmssim::Task::getNextActivationOffset to ensure that this method
     * is called at the right time.
     * @param now current simulated time
     * @return the freshly activated job
     */
    virtual Job* spawnHook(TmsTime now) = 0;

    /**
     * This method is called by #tmssim::Task::spawnJob, if a new job was
     * created.
     * @param now current simulated time
     * @return The offset relative to the current time when the next job will be
     * available
     */
    virtual TmsTimeInterval getNextActivationOffset(TmsTime now) = 0;

    /**
     * This method is called by #tmssim::Task::completeJob.
     * Implement any cleaning up here, including freeing the memory that was
     * allocated for the completed Job.
     * @param job the completed job
     * @param now current simulated time
     */
    virtual bool completionHook(Job *job, TmsTime now) = 0;

    /**
     * This method is called by #tmssim::Task::cancelJob.
     * @param job the cancelled job
     * @return true, if the job was successfully cleaned up, false if the
     * job cancellation led to an error state for the task
     * @see #tmssim::Task::completionHook
     */
    virtual bool cancelHook(Job *job) = 0;

    /// @}
    
    // VERBOSE HOOK
    /// @name verbosity
    /// @{
    /**
     * This method shall return a one-letter short ID that identifies the
     * concrete task implementation. Implement this function in any
     * subclass.
     */
    virtual std::string getShortId(void) const = 0;
    
    /// @}
    
    // FAST RO ACCESS
    /// @name Fast access to task properties
    /// @{
    const unsigned int& id; ///< fast ro access to task id
    const TmsTimeInterval& executionTime;  ///< fast ro access to execution time
    const TmsTimeInterval& relDeadline; ///< fast ro access to critical time
    const TmsPriority& priority; ///< fast ro access
    /// @}

    // STATISTICS
    /// @name Fast access to execution statistics
    /// @{
    const int& activations; ///< fast ro access @todo move to statistics class
    const int& completions; ///< fast ro access @todo move to statistics class
    const int& cancellations; ///< fast ro access @todo move to statistics class
    const int& misses; ///< fast ro access @todo move to statistics class
    const int& preemptions; ///< fast ro access @todo move to statistics class
    /// @}
    
    /**
      Notify the delay counter about a finished job execution.
      This function is called the #completeJob and #cancelJob methods.
      Depending on the u parameter, either #recordDelay (u <= 0) or
      #recordNoDelay are called.
      @param u the utility that was achieved by a job execution
      @todo The binary notion of delay might prove not useful. It might be
      helpful to make these methods virtual to allow them to be overridden
      by more expressive functions.
    */
    void record(double u);
    /// record a delayed/cancelled job execution
    void recordDelay();
    /// record a successful job execution
    void recordNoDelay();
    
  private:
    unsigned int _id; ///< task id
    TmsTimeInterval _executionTime; ///< execution time
    TmsTimeInterval _relDeadline; ///< relative critical time
    UtilityCalculator* _uc; ///< the task's UtilityCalculator
    UtilityAggregator* _ua; ///< the task's UtilityAggregator
    int _activations; ///< @todo move to statistics class
    int _completions; ///< @todo move to statistics class
    int _cancellations; ///< @todo move to statistics class
    int _misses; ///< @todo move to statistics class
    int _preemptions; ///< @todo move to statistics class
    int _execCancellations; ///< count cancellations after task started execution @todo move to statistics class
    TmsTime _ecPerformanceLost; ///< cycles lost due to exec cancellations
    TmsPriority _priority; ///< task (static) priority
    TmsTime _nextActivation; ///< when will the next job be generated?
    
    double _lastValue; ///< utility of last job execution
    
    // Delay counter statistics members
    size_t delayCounter; ///< counts current successive delays/cancellations
    std::vector<int> delayCounters;
    //size_t currentSize; // FIXME: remove
  };

  typedef std::vector<Task*> Taskset;

  std::ostream& operator << (std::ostream& ost, const Task& task);

} // NS tmssim

#endif // !CORE_TASK_H
