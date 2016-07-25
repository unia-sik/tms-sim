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
 * $Id: mkpsimulation.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file mkpsimulation.h
 * @brief Simulation of mk-tasks with fixed (m,k)-patterns
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVAL_MKPSIMULATION_H
#define MKEVAL_MKPSIMULATION_H

#include <mksimulation.h>

namespace tmssim {

  class MkpSimulation : public MkSimulation {
  public:
    MkpSimulation(std::list<MkTask*> _mkTasks, Scheduler* _scheduler,
		  const std::string& _allocId);
    
    ~MkpSimulation();

    virtual bool simulate();
    virtual std::string getInfoMessage();
    virtual std::string getSimulationMessage();
    virtual bool getSuccess() { return success; }

  protected:
    void calcFeasibilityPeriod();
    
  private:
    void initData();

    bool simulated;
    bool success;

    bool sTestSuccess;
    
    TmsTime feasibilityPeriod;
  };

  
  MkSimulation* MkpSimulationAllocator(std::list<MkTask*> _mkTasks,
				       Scheduler* _scheduler,
				       const std::string& _allocId);
  
} // NS tmssim

#endif // !MKEVAL_MKPSIMULATION_H
