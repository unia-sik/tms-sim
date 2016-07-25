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
 * $Id: gstsimulation.h 1432 2016-07-15 10:43:04Z klugeflo $
 * @file gstsimulation.h
 * @brief (m,k)-firm task set for simulation
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVAL_MKEXTTASKSET_H
#define MKEVAL_MKEXTTASKSET_H

#include <cstdint>
#include <list>

#include <core/scheduler.h>
#include <core/simulation.h>
#include <taskmodels/mktask.h>
#include <mkeval/mksimulation.h>

namespace tmssim {

  /**
   * @todo rename to GstSimulation
   */
  class GstSimulation : public MkSimulation {
  public:
    GstSimulation(std::list<MkTask*> _mkTasks, Scheduler* _scheduler,
		 std::string _allocId);

    ~GstSimulation();
    
    /**
     * Simulate the task set either until an error ((m,k)-violation)
     * occurs, or a whole (m,k)-hyperperiod was simulated without error
     * @return true, if simulation was successful (i.e. task set is schedulable), false if an error occurred
     */
    bool simulate();

    unsigned getHpCount() const { return hpCount; }

    const CompressedMkState* getLastMkStates() const { return states.back(); }

    TmsTime getHyperPeriod() const { return hyperPeriod; }
    TmsTime getMkFeasibilityMultiplicator() const { return mkFeasibilityMultiplicator; }
    TmsTime getMkFeasibilityInterval() const { return mkFeasibilityInterval; }

    const Simulation* getSimulation() const { return simulation; }
    bool stateRecurred() const { return recurringState; }

    bool reducedStateRecurred() const { return recurringReducedState; }
    TmsTime getReducedStateHyperPeriod() const { return reducedStateHyperPeriod; }
    TmsTime getReducedStateCycle() const { return reducedStateCycle; }

    virtual bool getSuccess() { return success; }

    virtual std::string getInfoMessage();
    virtual std::string getSimulationMessage();

  private:

    void initData();
    /// check whether the current (m,k)-state of all tasks has occurred before
    bool checkState() const;
    /// record the current (m,k)-state of all tasks
    void recordState();

    bool checkReducedState();
    
    //std::list<MkTask*> mkTasks;
    //Scheduler* scheduler;
    //size_t nTasks;
    
    double realUtilisation;
    double mkUtilisation;

    TmsTime hyperPeriod;
    TmsTimeInterval mkFeasibilityMultiplicator;
    TmsTime mkFeasibilityInterval;
    
    Taskset* simTasks;
    //Simulation* simulation;
    unsigned hpCount;

    /// size of each array is #nTasks
    std::list<CompressedMkState*> states;
    bool recurringState;

    /**
     * Reduced  (m,k)-states (leave out least recent entry)
     * size of each array is #nTasks
     */
    std::list<CompressedMkState*> reducedStates;

    bool recurringReducedState;
    TmsTime reducedStateHyperPeriod;
    TmsTime reducedStateCycle;

    bool simulated;
    bool success;
  };

  MkSimulation* GstSimulationAllocator(std::list<MkTask*> _mkTasks,
				       Scheduler* _scheduler,
				       const std::string& _allocId);

} // NS tmssim

#endif
