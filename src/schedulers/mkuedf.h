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
 * $Id: mkuedf.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file mkuedf.h
 * @brief HCUF-based (m,k)-firm real-time scheduling
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_MKUEDF_H
#define SCHEDULERS_MKUEDF_H 1

#include <schedulers/oedfmax.h>

namespace tmssim {

  class MKUEDFScheduler : public OEDFMaxScheduler {
  public:
    MKUEDFScheduler(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);
    virtual ~MKUEDFScheduler();
    
    virtual const std::string& getId(void) const;
    
  protected:
    /**
     * Contains work-around to avoid cancellation if job has already started
     * execution. In this case, the value is set to 0. Thus, the job is
     * discarded by MKUEDFScheduler::isCancelCandidate.
     */
    virtual double calcValue(TmsTime time, const Job *job) const;
    virtual bool isCancelCandidate(const Job* job, double value) const;

  };


  /**
   * @brief Generic allocator function
   */
  Scheduler* MKUEDFSchedulerAllocator(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);

} // NS tmssim

#endif // !SCHEDULERS_MKUEDF_H
