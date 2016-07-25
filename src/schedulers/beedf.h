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
 * $Id: beedf.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file beedf.h
 * @brief Best effort EDF scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_BEEDF_H
#define SCHEDULERS_BEEDF_H 1

#include <list>

#include <schedulers/oedfmin.h>

namespace tmssim {

  /**
    Implementation of Best-Effort EDF scheduler.
    Based on Jensen et al. RTSS 1985, Locke PhD 1986
  */
  class BEEDFScheduler : public OEDFMinScheduler {
  public:
    
    BEEDFScheduler();
    virtual ~BEEDFScheduler();
    
    virtual const std::string& getId(void) const;
    
  protected:
    virtual double calcValue(TmsTime time, const Job *job) const;
    
  };

  /**
   * @brief Generic allocator function
   */
  Scheduler* BEEDFSchedulerAllocator();
  
} // NS tmssim

#endif /* SCHEDULERS_BEEDF_H */

