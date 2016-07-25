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
 * $Id: fixedtimesimulation.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file fixedtimesimulation.h
 * @brief Simulate task set only for a fixed time
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVAL_FIXEDTIMESIMULATION_H
#define MKEVAL_FIXEDTIMESIMULATION_H

#include <mkeval/mksimulation.h>

namespace tmssim {

  class FixedTimeSimulation : public MkSimulation {
  public:
    FixedTimeSimulation(std::list<MkTask*> _mkTasks, Scheduler* _scheduler,
			const std::string& _allocId, TmsTime _steps);

    virtual ~FixedTimeSimulation();

    virtual bool simulate();
    virtual std::string getInfoMessage();
    virtual std::string getSimulationMessage();
    virtual bool getSuccess() { return success; }

    /**
     * It's really dirty to do it this way, but at the moment it's fastest
     * @todo make better
     */
    static void setSteps(TmsTime _steps) { theSteps = _steps; }
    static TmsTime getSteps() { return theSteps; }
  protected:

  private:
    /// @todo remove this global
    static TmsTime theSteps;
    
    bool simulated;
    bool success;
    TmsTime steps;
  };

  MkSimulation* FixedTimeSimulationAllocator(std::list<MkTask*> _mkTasks,
					     Scheduler* _scheduler,
					     const std::string& _allocId);

} // NS tmssim

#endif // !MKEVAL_FIXEDTIMESIMULATION_H
