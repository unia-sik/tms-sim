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
 * $Id: gmuamk.h 1421 2016-06-22 07:46:32Z klugeflo $
 * @file gmuamk.h
 * @brief gMUA-MK scheduler for single processor
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 * @see J.-H. Rhu, J.-H. Sun, K. Kim, H. Cho and J.K. Park, Utility accrual
 *      real-time scheduling for (m,k)-firm deadline-constrained streams on
 *      multiprocessors, Electronics Letters, March 2011
 */

#ifndef SCHEDULERS_GMUAMK_H
#define SCHEDULERS_GMUAMK_H 1

#include <schedulers/oedfmin.h>

namespace tmssim {

  /**
   * @brief Single-processor implementation of the gMUA-MK scheduler.
   *
   * Jobs with min value are directly cancelled! Original algorithm just
   * postpones these to the end of the schedule. This makes only sense, if
   * job execution times vary. In this implementation, these jobs
   * would starve!
   */
  class GMUAMKScheduler : public OEDFMinScheduler {
  public:
    GMUAMKScheduler(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);
    virtual ~GMUAMKScheduler();
    
    virtual const std::string& getId(void) const;
    
  protected:
    virtual double calcValue(TmsTime time, const Job *job) const;
    virtual bool isCancelCandidate(const Job* job, double value) const;

  };


  /**
   * @brief Generic allocator function
   */
  Scheduler* GMUAMKSchedulerAllocator(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);

} // NS tmssim

#endif // !SCHEDULERS_GMUAMK_H
