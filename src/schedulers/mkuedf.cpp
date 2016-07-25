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
 * $Id: mkuedf.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file mkuedf.cpp
 * @brief HCUF-based (m,k)-firm real-time scheduling
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <schedulers/mkuedf.h>

namespace tmssim {


  static const std::string myId = "MKUEDFScheduler";
  
  MKUEDFScheduler::MKUEDFScheduler(const SchedulerConfiguration& schedulerConfiguration)
    : OEDFMaxScheduler(schedulerConfiguration) {
  }


  MKUEDFScheduler::~MKUEDFScheduler() {
  }

  
  const std::string& MKUEDFScheduler::getId(void) const {
    return myId;
  }


  double MKUEDFScheduler::calcValue(__attribute__((unused)) TmsTime time, const Job *job) const {
    if (!myConfig.execCancellations
	&& job->getRemainingExecutionTime() < job->getExecutionTime())
      return 0;
    else
      return job->getPossibleFailHistoryValue();
  }
 

  bool MKUEDFScheduler::isCancelCandidate(const Job* job, double value) const {
    return value >= 1 && OEDFScheduler::isCancelCandidate(job, value);
  }


  Scheduler* MKUEDFSchedulerAllocator(const SchedulerConfiguration& schedulerConfiguration) {
    return new MKUEDFScheduler(schedulerConfiguration);
  }

} // NS tmssim
