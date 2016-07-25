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
 * $Id: dbpedf.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file dbpedf.h
 * @brief Utility-based EDF using distance from fail state
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_DBPEDF_H
#define SCHEDULERS_DBPEDF_H 1

#include <schedulers/oedfmax.h>

namespace tmssim {

  class DBPEDFScheduler : public OEDFMaxScheduler {
  public:
    DBPEDFScheduler(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);
    virtual ~DBPEDFScheduler();
    
    virtual const std::string& getId(void) const;
    
  protected:
    virtual double calcValue(TmsTime time, const Job *job) const;
    virtual bool isCancelCandidate(const Job* job, double value) const;

  };


  /**
   * @brief Generic allocator function
   */
  Scheduler* DBPEDFSchedulerAllocator(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);


} // NS tmssim

#endif // !SCHEDULERS_DBPEDF_H
