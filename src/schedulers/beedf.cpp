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
 * $Id: beedf.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file beedf.cpp
 * @brief Implementation of best effort EDF scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */
/**
 * $Id: beedf.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * Timing Models Scheduling Simulator
 */

#include <cfloat>
#include <climits>
#include <cstddef>
#include <list>

#include <iostream>

#include <schedulers/beedf.h>

using namespace std;

namespace tmssim {

  static const std::string myId = "BEEDFScheduler";
  
  
  BEEDFScheduler::BEEDFScheduler()
    : OEDFMinScheduler() { //EDFScheduler() {
  }
  
  
  BEEDFScheduler::~BEEDFScheduler() {
  }
  
  
  const string& BEEDFScheduler::getId(void) const {
    return myId;
  }
  
  
  double BEEDFScheduler::calcValue(TmsTime time, const Job *job) const {
    return job->getPossibleExecValue(time) / job->getExecutionTime();
  }


  Scheduler* BEEDFSchedulerAllocator() { return new BEEDFScheduler; }
  
} // NS tmssim
