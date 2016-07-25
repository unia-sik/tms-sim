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
 * $Id: fixedtimesimulation.cpp 1421 2016-06-22 07:46:32Z klugeflo $
 * @file fixedtimesimulation.cpp
 * @brief Simulate task set only for a fixed time
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <mkeval/fixedtimesimulation.h>

using namespace std;

namespace tmssim {

  TmsTime FixedTimeSimulation::theSteps = 0;
  
  FixedTimeSimulation::FixedTimeSimulation(list<MkTask*> _mkTasks,
					   Scheduler* _scheduler,
					   const string& _allocId,
					   TmsTime _steps)
    : MkSimulation(_mkTasks, _scheduler, _allocId),
      simulated(false),
      success(false),
      steps(_steps)
  {
  }

  
  FixedTimeSimulation::~FixedTimeSimulation() {
  }
  

  bool FixedTimeSimulation::simulate() {
    Simulation::ExitCondition ec;
    ec = simulation->run(steps);
    success = (ec == 0);
    simulated = true;
    return success;
  }

  
  string FixedTimeSimulation::getInfoMessage() {
    ostringstream oss;
    oss << "Fixed simulation time: " << steps << endl;
    return oss.str();
  }

  
  string FixedTimeSimulation::getSimulationMessage() {
    ostringstream oss;
    oss << getAllocId() << ":";

    if (!simulated) {
      oss << "Not yet simulated!";
      return oss.str();
    }

    if (success) {
      oss << " success!";
    }
    else {
      oss << " failed in cycle "
	  << simulation->getTime();
    }
    
    return oss.str();
  }


  MkSimulation* FixedTimeSimulationAllocator(list<MkTask*> _mkTasks,
					     Scheduler* _scheduler,
					     const string& _allocId) {
    return new FixedTimeSimulation(_mkTasks, _scheduler, _allocId,
				   FixedTimeSimulation::getSteps());
  }


} // NS tmssim
