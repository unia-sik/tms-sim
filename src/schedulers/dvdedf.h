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
 * $Id: dvdedf.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file dvdedf.h
 * @brief Dynamic value density scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_DVDEDF_H
#define SCHEDULERS_DVDEDF_H 1

#include <list>

#include <schedulers/oedfmin.h>

namespace tmssim {

  /**
    Implementation of Dynamic Value Density Scheduler.
    See Saud A. Aldarmi and Alan Burns, Dynamic value-density for scheduling
    real-time systems, ECRTS 1999
  */
  class DVDEDFScheduler : public OEDFMinScheduler {
    
  public:
    
    DVDEDFScheduler();
    virtual ~DVDEDFScheduler();
    
    virtual const std::string& getId(void) const;
    
  protected:
    virtual double calcValue(TmsTime time, const Job *job) const;
    
  };

  /**
   * @brief Generic allocator function
   */
  Scheduler* DVDEDFSchedulerAllocator();

} // NS tmssim

#endif /* SCHEDULERS_DVDEDF_H */
