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
 * $Id: fppnat.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file fppnat.cpp
 * @brief Fixed Priority Preemptive Scheduler with "natural" priority ordering
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <schedulers/fppnat.h>
#include <utils/tlogger.h>

#include <cassert>
#include <iostream>
using namespace std;

namespace tmssim {

  static const std::string myId = "FPPNatScheduler";
  
  FPPNatScheduler::FPPNatScheduler() : ALDScheduler() {
  }


  FPPNatScheduler::~FPPNatScheduler() {
  }

    
  void FPPNatScheduler::enqueueJob(Job *job) {
    tDebug() << "Enqueueing job " << *job << " @ " << job << " T@ " << job->getTask();
    assert(job->getTask() != NULL);
    assert((long long)job->getTask() < 0x800000000000LL);
    std::list<Job*>::iterator it = mySchedule.begin();
    while ( it != mySchedule.end()
	    && *it != NULL
	    && (*it)->getPriority() >= job->getPriority() ) {
      it++;
    }
    mySchedule.insert(it, job);
    //scheduleChanged = true;
    notifyScheduleChanged();
    dlmon.addJob(job);
    //return true;
  }


  int FPPNatScheduler::schedule(TmsTime now, ScheduleStat& scheduleStat) {
    printSchedule();
    Job* missJob = NULL;
    while ( (missJob = (Job*)dlmon.check(now)) != NULL) {
      scheduleStat.cancelled.push_back((Job*)missJob);
      removeJob(missJob);
    }
    return 0;
  }

  Job* FPPNatScheduler::dispatch(TmsTime now, DispatchStat& dispatchStat) {
    Job* job = ALDScheduler::dispatch(now, dispatchStat);
    const Job* fin = dispatchStat.finished;
    if (fin != NULL) {
      if (fin != dlmon.removeJob(fin)) {
	tError() << "Finished job " << job << " " << *job << " not found in dlmon!";
      }
      else {
	tDebug() << "Removed finished job " << job << " " << *job << " from dlmon!";
      }
    }
    return job;
  }


  const std::string& FPPNatScheduler::getId(void) const {
    return myId;
  }


  Scheduler* FPPNatSchedulerAllocator() { return new FPPNatScheduler; }

} // NS tmssim
