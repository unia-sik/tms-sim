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
 * $Id: gdpa.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file gdpa.cpp
 * @brief Guaranteed Dynamic Priority Assigment Scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <schedulers/gdpa.h>
#include <utils/tlogger.h>
#include <utils/logger.h>

#include <cassert>

namespace tmssim {

  static const std::string myId = "GDPAScheduler";

  GDPAScheduler::GDPAScheduler(const SchedulerConfiguration& schedulerConfiguration)
    : EDFScheduler(schedulerConfiguration), readyQueueChanged(false) {
  }


  GDPAScheduler::~GDPAScheduler() {
    // ALDScheduler will cleanup the mySchedule list
    // BUT: not every job may be in this list!
    mySchedule = readyQueue;
  }


  void GDPAScheduler::enqueueJob(Job *job) {
    assert(job->getTask() != NULL);
    //assert((long long)job->getTask() < 0x800000000000LL);
    readyQueue.push_back(job);
    readyQueueChanged = true;
    jobEnqueued(job);
  }


  bool GDPAScheduler::hasPendingJobs(void) const {
    return readyQueue.size() > 0;
  }

  /*
  int GDPAScheduler::initStep(TmsTime now, ScheduleStat& scheduleStat) {
    if (!myConfig.dlMissCancellations) {
      return 0;
    }

    Job* missJob = NULL;
    while ( (missJob = (Job*)dlmon.check(now)) != NULL) {
      if ( !myConfig.execCancellations &&
	   missJob->getRemainingExecutionTime() < missJob->getExecutionTime() ) {
	LOG(LOG_CLASS_SCHEDULER) << "Moving job " << missJob << " (" << *missJob
		 << ") to execMissJobs list (now = " << now << ")!";
	execMissJobs.push_back(missJob);
      }
      else {
	scheduleStat.cancelled.push_back((Job*)missJob);
	internalRemoveJob(missJob);
      }
    }

    return 0;
  }
  */

  int GDPAScheduler::schedule(__attribute__((unused)) TmsTime now, __attribute__((unused)) ScheduleStat& scheduleStat) {
    list<Job*>::iterator it;
    // Check feasibility of all jobs
    // this is already done by ALDScheduler::initStep via the deadlinemonitor!
    /*
    it = readyQueue.begin();
    while (it != readyQueue.end()) {
      if ((*it)->getLatestStartTime() < now) {
	Job* cancel = *it;
	it = readyQueue.erase(it);
	tDebug() << "Removing Job " << *cancel;
	scheduleStat.cancelled.push_back(cancel);
      }
      else {
	++it;
      }
    }
    */
    
    // sort by shortest distance
    list<Job*> sdfList;
    for (it = readyQueue.begin();
	 it != readyQueue.end(); ++it) {
      Job* job = *it;
      int distance = job->getTask()->getDistance();
      list<Job*>::iterator ins = sdfList.begin();
      while ( ins != sdfList.end()
	      && *ins != NULL
	      && (*ins)->getTask()->getDistance() <= distance ) {
	ins++;
      }
      sdfList.insert(ins, job);
    }
    // create feasible EDF schedule
    mySchedule.clear();
    for (it = sdfList.begin(); it != sdfList.end(); it++) {
      Job* job = *it;
      std::list<Job*>::iterator ins = mySchedule.begin();
      while ( ins != mySchedule.end()
	      && *ins != NULL
	      && (*ins)->getAbsDeadline() <= job->getAbsDeadline() ) {
	ins++;
      }
      ins = mySchedule.insert(ins, job);
      // now check feasibility
      const Job* fjob = checkEDFSchedule(now);
      if (fjob != NULL) {
	mySchedule.erase(ins);
      }
    }
    readyQueueChanged = false;
    notifyScheduleChanged();
    return 0;
  }


  const std::string& GDPAScheduler::getId(void) const {
    return myId;
  }


  void GDPAScheduler::jobFinished(Job *job) {
    list<Job*>::iterator it;
    it = readyQueue.begin();
    while (it != readyQueue.end() && *it != job)
      ++it;
    if (it != readyQueue.end()) { // found
      readyQueue.erase(it);
    }
  }


  Scheduler* GDPASchedulerAllocator(const SchedulerConfiguration& schedulerConfiguration) {
    return new GDPAScheduler(schedulerConfiguration);
  }

} // NS tmssim
