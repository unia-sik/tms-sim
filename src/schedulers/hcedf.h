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
 * $Id: hcedf.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file hcedf.h
 * @brief History-cognisant overload EDF scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_HCEDF_H
#define SCHEDULERS_HCEDF_H 1

#include <list>

#include <schedulers/oedfmax.h>

namespace tmssim {

  /**
    History-cognisant overload EDF scheduler.
    Removes jobs with maximum value of task utility from schedule if overload
    is detected.
    See Kluge/Gerdes/Haas/Ungerer @ RTNS 2013.
  */
  class HCEDFScheduler : public OEDFMaxScheduler {
    
  public:
    
    HCEDFScheduler();
    virtual ~HCEDFScheduler();
    
    virtual const std::string& getId(void) const;
    
  protected:
    virtual double calcValue(TmsTime time, const Job *job) const;
    
  };


  /**
   * @brief Generic allocator function
   */
  Scheduler* HCEDFSchedulerAllocator();

} // NS tmssim

#endif /* SCHEDULERS_HCEDF_H */

