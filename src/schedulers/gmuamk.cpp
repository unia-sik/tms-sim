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
 * $Id: gmuamk.cpp 1421 2016-06-22 07:46:32Z klugeflo $
 * @file gmuamk.cpp
 * @brief gMUA-MK scheduler for single processor
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 * @see J.-H. Rhu, J.-H. Sun, K. Kim, H. Cho and J.K. Park, Utility accrual
 *      real-time scheduling for (m,k)-firm deadline-constrained streams on
 *      multiprocessors, Electronics Letters, March 2011
 */

#include <schedulers/gmuamk.h>

namespace tmssim {

  static const std::string myId = "GMUAMKScheduler";
  
  GMUAMKScheduler::GMUAMKScheduler(const SchedulerConfiguration& schedulerConfiguration)
    : OEDFMinScheduler(schedulerConfiguration) {
  }


  GMUAMKScheduler::~GMUAMKScheduler() {
  }

  
  const std::string& GMUAMKScheduler::getId(void) const {
    return myId;
  }


  double GMUAMKScheduler::calcValue(TmsTime time, const Job *job) const {
    return job->getPossibleExecValue(time) / (job->getExecutionTime() * job->getTask()->getDistance());
  }
 

  bool GMUAMKScheduler::isCancelCandidate(const Job* job, double value) const {
    return value >= 1 && OEDFScheduler::isCancelCandidate(job, value);
  }


  Scheduler* GMUAMKSchedulerAllocator(const SchedulerConfiguration& schedulerConfiguration) {
    return new GMUAMKScheduler(schedulerConfiguration);
  }
  
} // NS tmssim
