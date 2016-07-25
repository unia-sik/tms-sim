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
 * $Id: mksimulation.h 1421 2016-06-22 07:46:32Z klugeflo $
 * @file mksimulation.h
 * @brief Abstract superclass for (m,k)-simulations with exact schedulability
 *        test
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVAL_MKSIMULATION_H
#define MKEVAL_MKSIMULATION_H

#include <list>
#include <string>

#include <core/scheduler.h>
#include <core/simulation.h>
#include <taskmodels/mktask.h>

namespace tmssim {

  class MkSimulation {
  public:
    MkSimulation(std::list<MkTask*> _mkTasks, Scheduler* _scheduler,
		 const std::string& _allocId);

    virtual ~MkSimulation();
    
    const Simulation* getSimulation() const { return simulation; }
    const std::string& getAllocId() { return allocId; }

    virtual bool simulate() = 0;
    virtual std::string getInfoMessage() = 0;
    virtual std::string getSimulationMessage() = 0;
    virtual bool getSuccess() = 0;

    static MkSimulation* simulate(MkSimulation* mkSimulation) {
      mkSimulation->simulate();
      return mkSimulation;
    }

    //const MkTaskset* getMkTaskset() const { return mkTaskset; }
    const std::list<MkTask*> getMkTasks() const { return mkTasks; }
    
  protected:

    std::list<MkTask*> mkTasks;
    size_t nTasks;

    Taskset* simTasks;
    Simulation* simulation;

  private:
    Scheduler* scheduler;
    std::string allocId;

  };

  
} // NS tmssim


#endif // !MKEVAL_MKSIMULATION_H
