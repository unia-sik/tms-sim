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
 * $Id: ald.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file ald.h
 * @brief Abstract List Dispatch scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_ALD_H
#define SCHEDULERS_ALD_H 1

#include <core/scheduler.h>
#include <core/scconfig.h>
#include <core/deadlinemonitor.h>

#include <list>


namespace tmssim {

  /**
   * @brief Abstract List Dispatch scheduler.
   *
   * This (abstract) class manages dispatching from a list which contains
   * active jobs in execution order. It also keeps track of jobs' deadlines
   * and cancels jobs that cannot keep their deadline (if configured).
   */
  class ALDScheduler : public Scheduler {

  public:
    ALDScheduler(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);//Logger* _logger = noLogPtr);

    /**
     * The destructor deletes all jobs that are still kept in the
     * ALDScheduler::mySchedule list. This is only the last resort,
     * actually, child classes should take care of cleaning up the list!
     */
    virtual ~ALDScheduler();

    /**
     * @brief Remove an active job from the current schedule.
     *
     * @param job the job to remove
     * @return The removed job, or <b>NULL</b> if the job is not in the schedule
     */
    virtual const Job* removeJob(const Job *job);

    /**
     * @brief Prepare an execution step.
     *
     * Especially, perform cancellation of jobs that cannot keep their
     * deadlines. This must be done before any new activation to have
     * consisten task states.
     *
     * This method regards jobs that are queued for deadline monitoring.
     * It notifies subclass implementations via the jobFinished() method.
     *
     * @param now Time
     * @param[out] scheduleStat stores cancelled jobs
     * @return 0
     */
    virtual int initStep(TmsTime now, ScheduleStat& scheduleStat);

    
    /**
     * @brief Dispatch one job from the schedule for execution.
     *
     * This function takes the first element of the ALDScheduler::mySchedule
     * list and executes it for one cycle. If the ALDScheduler::mySchedule
     * list was changed through calls to Scheduler::enqueueJob or
     * Scheduler::schedule, your implementation must ensure to set the
     * ALDScheduler::scheduleChanged variable to true.
     * The ALDScheduler::mySchedule list may be altered: If the job finished
     * execution, it is removed from the list.
     *
     * If you plan to implement a non-preemptive scheduler, then make sure
     * to enqueue newly activated jobs <b>after</b> the first job in
     * ALDScheduler::mySchedule if that one has already started execution!
     *
     * @param now Point in time, succeeding calls to this functions must have
     * increasing times
     * @param dispatchStat Container for statistics
     * @return finished job, NULL (running/idling), or ESC_DISP error code
     */
    virtual Job* dispatch(TmsTime now, DispatchStat& dispatchStat);

    /**
     * @brief Check wether there are active jobs.
     * @return <b>true</b> if there are active jobs.
     */
    virtual bool hasPendingJobs(void) const;

    /**
     * @brief Print current schedule.
     */
    virtual void printSchedule() const;

  protected:

    /**
     * @brief Notification about newly arrived job.
     *
     * This method must be called by any implementation of
     * Scheduler::enqueueJob. It is responsible for deadline monitoring.
     * @param job The newly arrived job.
     */
    virtual void jobEnqueued(Job *job);
    
    /**
     * @brief Notification about finished Job.
     *
     * This method is called by dispatch() (if a job has finished execution)
     * and initStep() (if a job was cancelled due to impeding deadline miss).
     * Overwrite it in your scheduler implementation, if you are keeping
     * jobs stored additionally in other data structures. Here, you
     * get a chance to remove references to keep your data structures
     * consistent.
     * @param job the finished job
     */
    virtual void jobFinished(Job *job);


    /*
     * @brief Notification that a job was cancelled.
     *
     * Call this function any time your scheduler implementation decides
     * to cancel a job.
     * @param removedJob the job that was cancelled
     * @todo Remove method
     */
    //virtual void notifyJobRemoved(const Job *removedJob);


    /**
     * @brief Notification that the schedule changed.
     *
     * Call this function any time your scheduler implementation
     * changes the #mySchedule list.
     */
    virtual void notifyScheduleChanged();


    /**
     * @brief List holding the current schedule.
     *
     * Use this list to store the schedule that is generated by your
     * implementation. Dispatching is performed from the front.
     * This list may be altered by the ALDScheduler::dispatch() method
     */
    std::list<Job*> mySchedule;

    
    /**
     * @brief Configuration data for execution behaviour.
     */
    SchedulerConfiguration myConfig;

    
    /**
     * @brief Monitor jobs for impending deadline misses.
     */
    DeadlineMonitor dlmon;

    
    /**
     * @brief Jobs that miss their deadline but must still be executed.
     */
    std::list<const Job*> execMissJobs;

    
  private:
    /**
     * @brief Finish removal of a job that was earlier found to miss its
     * deadline.
     *
     * @param job the job to remove
     * @return The removed job, or <b>NULL</b> if the job is not in the schedule
     */
    const Job* internalRemoveJob(const Job *job);
    
    /**
     * @brief The Job that is currently being executed/was executed
     * in the last cycle
     *
     * If you cancel this job in one of your implementations, make sure to
     * update this variable (to NULL).
     */
    Job* currentJob;

    /**
     * @brief Notification that the schedule has changed
     * Set this variable to true any time the ALDScheduler::mySchedule
     * list changed.
     */
    bool scheduleChanged;

  };

} // NS tmssim

#endif /* !SCHEDULERS_ALD_H */
