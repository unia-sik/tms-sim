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
 * $Id: deadlinemonitor.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file deadlinemonitor.h
 * @brief Deadline Monitor for use in scheduler implementations
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef CORE_DEADLINEMONITOR_H
#define CORE_DEADLINEMONITOR_H 1

#include <core/scobjects.h>
#include <list>

namespace tmssim {

  /**
   * @brief Deadline Monitor for use in scheduler implementations.
   * 
   * Pending deadline misses are identified as soon as the latest possible
   * start time for job has elapsed.
   */
  class DeadlineMonitor {
  public:
    DeadlineMonitor();
    ~DeadlineMonitor();

    
    /**
     * @brief Add job to the monitoring list
     */
    void addJob(const Job* job);

    
    /**
     * @brief Check if any job will miss its deadline.
     *
     * This function checks, whether any job in the list will miss its
     * deadline at the current time. If there is such a job, it will
     * be removed from the list and be returned to the caller. To remove
     * all jobs that will miss their deadlines from the list, call this
     * method until it returns NULL.
     * @param now the current time
     * @return NULL, if there is no job that will miss its deadline, else
     * a job will be returned.
     */
    const Job* check(TmsTime now);

    
    /**
     * @brief notify the monitor that a job was executed for some time,
     * but is not yet finished.
     *
     * This function adjusts the job's latest starting time and, if
     * necessary, reorders the monitoring list.
     */
    const Job* jobExecuted(const Job *job);

    
    /**
     * @brief Remove a job from the monitor (because it e.g. has finished
     * execution)
     * @param job The job to remove
     * @return the job on success, <b>NULL</b> else
     */
    const Job* removeJob(const Job* job);

    
  private:
    /**
     * Contains current jobs ordered increasingly by their latest
     * starting times.
     */
    std::list<const Job*> jobs;
  };

} // NS tmssim

#endif /* !CORE_DEADLINEMONITOR_H */
