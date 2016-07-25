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
 * $Id: phcedf.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file phcedf.h
 * @brief History-cognisant overload EDF scheduler (weight with remaining
 * execution time)
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_PHCEDF_H
#define SCHEDULERS_PHCEDF_H 1

#include <list>

#include <schedulers/oedfmax.h>

namespace tmssim {

  /**
    History-cognisant overload EDF scheduler with dynamic value density.
    Removes jobs with maximum value of task utility from schedule if overload
    is detected. The task utility is weighted by the remaining execution
    time of the job.
    See Kluge/Gerdes/Haas/Ungerer @ RTNS 2013.
  */
  class PHCEDFScheduler : public OEDFMaxScheduler {
    
  public:
    
    PHCEDFScheduler();
    virtual ~PHCEDFScheduler();
    
    virtual const std::string& getId(void) const;
    
  protected:
    virtual double calcValue(TmsTime time, const Job *job) const;
    
  };


  /**
   * @brief Generic allocator function
   */
  Scheduler* PHCEDFSchedulerAllocator();

} // NS tmssim

#endif /* SCHEDULERS_PHCEDF_H */

