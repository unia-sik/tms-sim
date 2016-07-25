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
 * $Id: mkpsimulation.cpp 1421 2016-06-22 07:46:32Z klugeflo $
 * @file mkpsimulation.cpp
 * @brief Simulation of mk-tasks with fixed (m,k)-patterns
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <mkeval/mkpsimulation.h>

#include <mkeval/mkgenerator.h>
#include <utils/tlogger.h>
#include <utils/tmsmath.h>

#include <vector>

using namespace std;

namespace tmssim {
  
  MkpSimulation::MkpSimulation(std::list<MkTask*> _mkTasks,
			       Scheduler* _scheduler,
			       const std::string& _allocId)
    : MkSimulation(_mkTasks, _scheduler, _allocId),
      simulated(false),
      success(false),
      sTestSuccess(false)
  {
    calcFeasibilityPeriod();
  }
  

  MkpSimulation::~MkpSimulation() {
  }
  

  bool MkpSimulation::simulate() {
    vector<MkTask*> vp;
    for (MkTask* task: mkTasks) {
      vp.push_back(task);
    }

    sTestSuccess = MkGenerator::testSufficientSchedulability(vp);
    success = sTestSuccess;
    
    if (!success) {
      Simulation::ExitCondition ec;
      // run simulation
      ec = simulation->run(feasibilityPeriod);
      success = (ec == 0);
    }
    
    simulated = true;
    return success;
  }
  
  
  string MkpSimulation::getInfoMessage() {
    ostringstream oss;
    oss << "FeasibilityPeriod: " << feasibilityPeriod << endl;
    return oss.str();
  }

  
  string MkpSimulation::getSimulationMessage() {
    ostringstream oss;
    oss << getAllocId() << ":";

    if (!simulated) {
      oss << "Not yet simulated!";
      return oss.str();
    }
    
    if (success) {
      oss << " success due to";
      if (sTestSuccess) {
	oss << " schedulability test";
      }
      else {
	oss << " simulation after " << simulation->getTime() << " cycles";
      }
    }
    else {
      oss << " failed in cycle "
	  << simulation->getTime();
    }
    oss << endl;
    
    return oss.str();
  }


  void MkpSimulation::calcFeasibilityPeriod() {
    feasibilityPeriod = 1;
    for (MkTask* task: mkTasks) {
      feasibilityPeriod = calculateLcm(feasibilityPeriod, task->getPeriod());
      feasibilityPeriod = calculateLcm(feasibilityPeriod, task->getK());
    }
  }
  
  
  void MkpSimulation::initData() {
    calcFeasibilityPeriod();
  }


  MkSimulation* MkpSimulationAllocator(std::list<MkTask*> _mkTasks,
				       Scheduler* _scheduler,
				       const std::string& _allocId) {
    return new MkpSimulation(_mkTasks, _scheduler, _allocId);
  }

  
} // NS tmssim
