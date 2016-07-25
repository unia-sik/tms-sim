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
 * $Id: fpp.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file fpp.cpp
 * @brief Fixed Priority Preemptive Scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <schedulers/fpp.h>
#include <utils/tlogger.h>

#include <cassert>
#include <iostream>
using namespace std;

namespace tmssim {

  static const std::string myId = "FPPScheduler";
  
  FPPScheduler::FPPScheduler(const SchedulerConfiguration& schedulerConfiguration)
    : ALDScheduler(schedulerConfiguration) {
  }


  FPPScheduler::~FPPScheduler() {
  }

  /*
  int FPPScheduler::initStep(TmsTime now, ScheduleStat& scheduleStat) {
    Job* missJob = NULL;
    while ( (missJob = (Job*)dlmon.check(now)) != NULL) {
      // FIXME: heed scConfig
      scheduleStat.cancelled.push_back((Job*)missJob);
      removeJob(missJob);
    }
    return 0;
  }
  */
    
  void FPPScheduler::enqueueJob(Job *job) {
    tDebug() << "Enqueueing job " << *job << " @ " << job << " T@ " << job->getTask();
    assert(job->getTask() != NULL);
    assert((long long)job->getTask() < 0x800000000000LL);
    std::list<Job*>::iterator it = mySchedule.begin();
    while ( it != mySchedule.end()
	    && *it != NULL
	    && (*it)->getPriority() <= job->getPriority() ) {
      it++;
    }
    mySchedule.insert(it, job);
    notifyScheduleChanged();

    //dlmon.addJob(job);
    jobEnqueued(job);
    //return true;
  }


  int FPPScheduler::schedule(__attribute__((unused)) TmsTime now, __attribute__((unused)) ScheduleStat& scheduleStat) {
    printSchedule();
    /*
    Job* missJob = NULL;
    while ( (missJob = (Job*)dlmon.check(now)) != NULL) {
      // If job has already executed, do not cancel!
      // FIXME: heed scConfig
      if (missJob->getRemainingExecutionTime() < missJob->getExecutionTime()) {
	tDebug() << "Moving job " << missJob << " (" << *missJob << ") to execMissJobs list (now = " << now << ")!";
	execMissJobs.push_back(missJob);
      }
      else {
	scheduleStat.cancelled.push_back((Job*)missJob);
	removeJob(missJob);
      }
    }
    */
    return 0;
  }

  /*
  Job* FPPScheduler::dispatch(TmsTime now, DispatchStat& dispatchStat) {
    Job* job = ALDScheduler::dispatch(now, dispatchStat);
    const Job* fin = dispatchStat.finished;
    if (fin != NULL) {
      if (fin->getAbsDeadline() <= now) {
	// job has missed its deadline, search in execMissJobs list
	//execMissJobs.remove(fin);
	list<Job*>::iterator it = execMissJobs.begin();
	while (it != execMissJobs.end()) {
	  if (*it == fin) {
	    break;
	  }
	  else {
	    ++it;
	  }
	}
	if (it != execMissJobs.end()) {
	  // found job!
	}
	else {
	  tError() << "Could not find DL-miss job " << job << " ("
		   << *job << ") in execMissJobs list (now = " << now << ")!";
	}
      }
      else if (fin != dlmon.removeJob(fin)) {
	tError() << "Finished job " << job << " " << *job << " not found in dlmon (now = " << now << ")!";
      }
      else {
	tDebug() << "Removed finished job " << job << " " << *job << " from dlmon!";
      }
    }
    else {
      const Job* exc = dispatchStat.executed;
      if (dlmon.jobExecuted(exc) != exc) {
	//tError() << "Notifying dlmon failed! " << exc << " " << *exc;
	// Don't care about this, the job should be in the execMissJobs list
      }
    }
    return job;
  }
  */

  const std::string& FPPScheduler::getId(void) const {
    return myId;
  }


  Scheduler* FPPSchedulerAllocator(const SchedulerConfiguration& schedulerConfiguration) { return new FPPScheduler(schedulerConfiguration); }

} // NS tmssim
