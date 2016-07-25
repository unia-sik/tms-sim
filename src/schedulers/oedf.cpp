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
 * $Id: oedf.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file oedf.cpp
 * @brief EDF scheduler with optional execution to resolve overloads
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <schedulers/oedf.h>

#include <utils/logger.h>

namespace tmssim {

  static const std::string myId = "OEDFScheduler";
  
  
  OEDFScheduler::OEDFScheduler(const SchedulerConfiguration& schedulerConfiguration)
    : EDFScheduler(schedulerConfiguration) {
  }
  
  
  OEDFScheduler::~OEDFScheduler() {
  }
  
  
  int OEDFScheduler::schedule(TmsTime now,
			      __attribute__((unused)) ScheduleStat& scheduleStat) {
    const Job* missJob;
    const Job* lastFoundJob = NULL;
    while ( (missJob = checkEDFSchedule(now)) != NULL
	    && missJob != lastFoundJob) {
      LOG(LOG_CLASS_SCHEDULER) << "Found DL-Miss job " << *missJob
			       << " (now=" << now << ")";
      list<Job*>::iterator current;
      double currentVal = getComparisonNeutral();
      TmsTime time = now; // FIXME: evolve???
      list<Job*>::iterator it;
      // search in jobs scheduled before missJob
      for (it = mySchedule.begin();
	   it != mySchedule.end() && *it != missJob; ++it) {
	Job* job = *it;
	double val = calcValue(time, job);
	time += job->getExecutionTime();
	LOG(LOG_CLASS_SCHEDULER) << "\tChecking " << *job << " with value "
				 << val << " (now=" << now << ")";
	if (!isCancelCandidate(job, val))
	  continue;
	if (compare(currentVal, val)) {
	  current = it;
	  currentVal = val;
	  LOG(LOG_CLASS_SCHEDULER) << "New candidate utility found: " << val
				   << " << @ job " << *job << " (now=" << now
				   << ")";
	}
      }
      if (*it == missJob) {
	// So far no cancidate was found - check if we can remove missJob
	double val = calcValue(time, missJob);
	if (isCancelCandidate(missJob, val) && compare(currentVal, val)) {
	  current = it;
	  currentVal = val;
	  LOG(LOG_CLASS_SCHEDULER) << "Selected missJob for cancellation";
	}
      }
      
      if (currentVal != getComparisonNeutral()) {
	// we have really found a job!
	Job* cancel = *current;
	LOG(LOG_CLASS_SCHEDULER) << "Removing Job " << *cancel
				 << " (now=" << now << ")";
	removeJob(cancel);
	scheduleStat.cancelled.push_back(cancel);
      }
      lastFoundJob = missJob;
    }
    return 0;
  }


  bool OEDFScheduler::isCancelCandidate(const Job* job, __attribute__((unused)) double value) const {
    if ( !myConfig.execCancellations &&
	 job->getRemainingExecutionTime() < job->getExecutionTime() ) {
      return false;
    }
    else {
      return true;
    }
  }


} // NS tmssim
