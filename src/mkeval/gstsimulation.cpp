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
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the copyright holder.
 */

/**
 * $Id: gstsimulation.cpp 1432 2016-07-15 10:43:04Z klugeflo $
 * @file gstsimulation.cpp
 * @brief (m,k)-firm task set for simulation
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <mkeval/gstsimulation.h>
#include <utils/bitstrings.h>
#include <utils/tmsmath.h>

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

namespace tmssim {

  GstSimulation::GstSimulation(std::list<MkTask*> _mkTasks, Scheduler* _scheduler,
			     std::string _allocId)
    : MkSimulation(_mkTasks, _scheduler, _allocId),
      //mkTasks(_mkTasks), scheduler(_scheduler), nTasks(_mkTasks.size()),
      hpCount(0), recurringState(false), recurringReducedState(false),
      reducedStateHyperPeriod(0), reducedStateCycle(0), simulated(false),
      success(false)
  {
    initData();
    /*
    simTasks = new Taskset;
    for (MkTask* task: mkTasks) {
      simTasks->push_back(task);
    }
    simulation = new Simulation(simTasks, scheduler);// Simulation::EC_CANCEL);
    */
  }


  GstSimulation::~GstSimulation() {
    for (CompressedMkState* cts: states) {
      delete[] cts;
    }
    states.clear();

    for (CompressedMkState* rts: reducedStates) {
      delete[] rts;
    }
    reducedStates.clear();

    //delete simulation;
  }

  
  bool GstSimulation::simulate() {
    Simulation::ExitCondition ec;
    bool finished = false;

    hpCount = 0;
    
    do {
      // store current state
      recordState();

      // run simulation
      ec = simulation->run(hyperPeriod);

      if (ec != 0) {
	finished = true;
      }
      else {
	// check current state
	if (checkState()) {
	  // state recurred, so we're finished
	  recurringState = true;
	  finished = true;
	}
	// For eval of reduced state (only meaningful for MKU!)
	if (checkReducedState() && !recurringReducedState) {
	  recurringReducedState = true;
	  reducedStateHyperPeriod = hpCount;
	  reducedStateCycle = simulation->getTime();
	}
      }
      
      ++hpCount;
      // TODO: should also check whether hpCount exceeds the number
      // necessary for exact test - then we know something went wrong
      if (hpCount > mkFeasibilityMultiplicator)
	finished = true;
    } while (!finished);

    simulated = true;
    success = (ec == 0);
    return success;
  }

  
  void GstSimulation::initData() {
    realUtilisation = 0;
    mkUtilisation = 0;
    hyperPeriod = 1;
    mkFeasibilityMultiplicator = 1;
    for (MkTask* task: mkTasks) {
      hyperPeriod = calculateLcm(hyperPeriod, task->getPeriod());
      double taskUtilisation = task->getExecutionTime() / task->getRelativeDeadline();
      realUtilisation += taskUtilisation;
      unsigned m = task->getM();
      unsigned k = task->getK();
      mkUtilisation += taskUtilisation * m / k;

      TmsTimeInterval taskFm = 0;
      for (size_t j = m; j <= k; ++j) {
	taskFm += binomialCoefficient(k, j);
      }
      mkFeasibilityMultiplicator *= taskFm;
    }
    mkFeasibilityInterval = hyperPeriod * mkFeasibilityMultiplicator;
  }
  

  void GstSimulation::recordState() {
    CompressedMkState* cts = new CompressedMkState[nTasks];
    CompressedMkState* rts = new CompressedMkState[nTasks];
    size_t i = 0;
    for (MkTask* task: mkTasks) {
      cts[i] = task->getMonitor().getState();
      rts[i] = task->getMonitor().getReducedState();
      ++i;
    }
    states.push_back(cts);
    reducedStates.push_back(rts);
  }


  bool GstSimulation::checkState() const {
    for (CompressedMkState* cts: states) {
      bool cmp = true;
      size_t i = 0;
      for (MkTask* task: mkTasks) {
	cmp &= cts[i] == task->getMonitor().getState();
	++i;
      }
      if (cmp)
	return true;
    }
    return false;
  }


  bool GstSimulation::checkReducedState() {
    for (CompressedMkState* rts: reducedStates) {
      bool cmp = true;
      size_t i = 0;
      cmp = true;
      for (MkTask* task: mkTasks) {
	cmp &= rts[i] == task->getMonitor().getReducedState();
	++i;
      }
      if (cmp)
	return true;
    }
    return false;
  }


  std::string GstSimulation::getInfoMessage() {
    ostringstream oss;
    oss << "Hyperperiod: " << getHyperPeriod() << endl;
    oss << "FeasibilityMultiplicator: " << getMkFeasibilityMultiplicator() << endl;
    oss << "FeasibilityInterval: " << getMkFeasibilityInterval() << endl;
    return oss.str();
  }


  string GstSimulation::getSimulationMessage() {
    ostringstream oss;

    oss << getAllocId() << ":";

    if (!simulated) {
      oss << "Not yet simulated!";
      return oss.str();
    }
    

    if (success) {
      oss << " success";
      if (stateRecurred())
	oss << " due to recurring state";
      
      oss <<" after " << getHpCount()-1
	  << " hyperperiods in cycle "
	  << getSimulation()->getTime() << "."
	  << " (EC: " << simulation->getResults().execCancellations
	  << "/" << simulation->getResults().ecPerformanceLost << ")";
      if (reducedStateRecurred()) {
	oss << " [RS @ " << getReducedStateHyperPeriod()
	    << " / " << getReducedStateCycle() << "]";
      }
      //oss << endl;
      
	
    }
    else { // (!success)
      oss << " failed after " << getHpCount()-1
	  << " hyperperiods in cycle "
	  << simulation->getTime() << "."
	  << " (EC: " << simulation->getResults().execCancellations << ")";
      //<< endl;
    }
    
    
    return oss.str();
  }


  MkSimulation* GstSimulationAllocator(std::list<MkTask*> _mkTasks,
				       Scheduler* _scheduler,
				       const std::string& _allocId) {
    return new GstSimulation(_mkTasks, _scheduler, _allocId);
  }
  
} // NS tmssim
