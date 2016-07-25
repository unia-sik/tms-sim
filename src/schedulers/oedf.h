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
 * $Id: oedf.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file oedf.h
 * @brief EDF scheduler with optional execution to resolve overloads
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_OEDF_H
#define SCHEDULERS_OEDF_H 1

#include <schedulers/edf.h>

namespace tmssim {

  class OEDFScheduler : public EDFScheduler {
  public:
    
    OEDFScheduler(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);
    virtual ~OEDFScheduler();
    virtual int schedule(TmsTime now, ScheduleStat& scheduleStat);

    // concrete subclasses need to re-implement this method!
    virtual const std::string& getId(void) const = 0;

  protected:
    /**
     * The comparison procedure needs an initialisation value, which
     * must be provided by this function.
     * @return the neutral element for the comparison
     */
    virtual double getComparisonNeutral() const = 0;

    /**
     * Compare wether a candidate task is better for removal than the
     * currently selected task.
     * @param current the value of the currently selected task
     * @param candidate the value of another candidate
     * @return true, if the candidate is fit better for removal than the
     * current task.
     */
    virtual bool compare(double current, double candidate) const = 0;

    /**
     * Calculate the comparison value of a job
     * @param time
     * @param job the job
     * @return the value of the job
     */
    virtual double calcValue(TmsTime time, const Job *job) const = 0;

    /**
     * Check whether the candidate may be cancelled based on its value.
     * This is a default implemenation that may be overwritten in
     * subclasses.
     * @param value
     * @todo param job: check exec cancellation; deferred - for now
     * work-around in MKUEDFScheduler::calcValue
     * @return true, if the candidate can be cancelled
     */
    virtual bool isCancelCandidate(const Job* job, double value) const;
  };
  

} // NS tmssim

#endif // !SCHEDULERS_OEDF_H
