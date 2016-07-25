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
 * $Id: oedfmin.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file oedfmin.h
 * @brief EDF with cancellation, minimum value cancel policy
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_OEDFMIN_H
#define SCHEDULERS_OEDFMIN_H 1

#include <list>

#include <schedulers/oedf.h>

namespace tmssim {
  
  /**
    This scheduler can handle overloads in an EDF scheduler. It removes jobs
    that have minimum value under some metric. This metric must be implemented
    in OEDFMinScheduler::calcValue
  */
  class OEDFMinScheduler : public OEDFScheduler {
    
  public:
    
    OEDFMinScheduler(const SchedulerConfiguration& schedulerConfiguration=DefaultSchedulerConfiguration);
    virtual ~OEDFMinScheduler();
    //virtual int schedule(int now, ScheduleStat& scheduleStat);

    virtual const std::string& getId(void) const;

  protected:
    //virtual double calcValue(int time, const Job *job) const = 0;
    //virtual bool isCancelCandidate(double value) const { return true; }

    virtual double getComparisonNeutral() const;
    virtual bool compare(double current, double candidate) const;
    
  };
  
} // NS tmssim

#endif /* SCHEDULERS_OEDFMIN_H */
