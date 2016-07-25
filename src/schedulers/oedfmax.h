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
 * $Id: oedfmax.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file oedfmax.h
 * @brief Overload EDF scheduler, maximum value jobs are removed
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_OEDFMAX_H
#define SCHEDULERS_OEDFMAX_H 1

#include <list>

#include <schedulers/oedf.h>

namespace tmssim {

  /**
    This scheduler can handle overloads in an EDF scheduler. It removes jobs
    that have maximum value under some metric. This metric must be implemented
    in OEDFMaxScheduler::calcValue
  */
  class OEDFMaxScheduler : public OEDFScheduler {
    
  public:
    
    OEDFMaxScheduler(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);
    virtual ~OEDFMaxScheduler();
    //virtual int schedule(int now, ScheduleStat& scheduleStat);
    
    virtual const std::string& getId(void) const;

  protected:
    //virtual double calcValue(int time, const Job *job) const = 0;
    //virtual bool isCancelCandidate(double value) const { return true; }

    virtual double getComparisonNeutral() const;
    virtual bool compare(double current, double candidate) const;

  };
  
} // NS tmssim

#endif /* SCHEDULERS_OEDFMAX_H */

