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
 * $Id: ald.cpp 1407 2016-05-31 07:30:51Z klugeflo $
 * @file ald.cpp
 * @brief Abstract List Dispatch scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <schedulers/ald.h>
#include <utils/tlogger.h>
#include <utils/logger.h>

#include <cassert>
#include <iostream>
using namespace std;

namespace tmssim {

  ALDScheduler::ALDScheduler(const SchedulerConfiguration& schedulerConfiguration)
    : Scheduler(), myConfig(schedulerConfiguration), currentJob(NULL), scheduleChanged(false) {
    LOG(LOG_CLASS_SCHEDULER) << "Created ALDScheduler with "
			     << "SCC EC: " << myConfig.execCancellations
			     << " DLMC: " << myConfig.dlMissCancellations;
  }


  ALDScheduler::~ALDScheduler() {
    list<Job*>::iterator it = mySchedule.begin();
    while (it != mySchedule.end()) {
      Job* job = *it;
      tInfo() << "ALDScheduler destroys unfinished job " << *job;
      //<< " in instance of " << getId();
      delete job;
      it = mySchedule.erase(it);
    }
  }


  const Job* ALDScheduler::removeJob(const Job *job) {
    list<Job*>::iterator it = mySchedule.begin();
    while (it != mySchedule.end() && *it != job)
      it++;

    if (it == mySchedule.end())
      return NULL;

    // actual removal deferred -- first make sure job is either in
    // deadlinemonitor or execMissJobs
    
    if (dlmon.removeJob(job) != job) {
      // job is not deadline-monitored, i.e. it has already missed its deadline
      // and thus should be in the execMissJobs list
      list<const Job*>::iterator ite = execMissJobs.begin();
      while ( ite != execMissJobs.end() && *ite != job ) {
	ite++;
      }
      if (ite == execMissJobs.end()) {
	// PROBLEM!
	tError() << "Job " << *job << " not found in DlMon+execMissJobs!";
	return NULL;
      }
      execMissJobs.erase(ite);
    }

    /*
    mySchedule.erase(it);


    notifyScheduleChanged();
    */
    return internalRemoveJob(job);
  }


  const Job* ALDScheduler::internalRemoveJob(const Job *job) {
    list<Job*>::iterator it = mySchedule.begin();
    while (it != mySchedule.end() && *it != job)
      it++;

    if (it == mySchedule.end())
      return NULL;

    mySchedule.erase(it);

    if (job == currentJob) {
      currentJob = NULL;
    }

    notifyScheduleChanged();
    
    return job;
  }
  
  
  int ALDScheduler::initStep(TmsTime now, ScheduleStat& scheduleStat) {
    if (!myConfig.dlMissCancellations) {
      return 0;
    }

    Job* missJob = NULL;
    while ( (missJob = (Job*)dlmon.check(now)) != NULL) {
      LOG(LOG_CLASS_SCHEDULER) << "Found miss job " << missJob << " (" << *missJob
			       << ")";
      if ( !myConfig.execCancellations &&
	   missJob->getRemainingExecutionTime() < missJob->getExecutionTime() ) {
	LOG(LOG_CLASS_SCHEDULER) << "Moving job " << missJob << " (" << *missJob
		 << ") to execMissJobs list (now = " << now << ")!";
	execMissJobs.push_back(missJob);
      }
      else {
	LOG(LOG_CLASS_SCHEDULER) << "Cancelling job " << missJob << " (" << *missJob;
	scheduleStat.cancelled.push_back((Job*)missJob);
	jobFinished(missJob);
	internalRemoveJob(missJob);
      }
    }

    return 0;
  }
  

  Job* ALDScheduler::dispatch(TmsTime now, DispatchStat& dispatchStat) {
    printSchedule();
    Job* prevJob = currentJob;
    if (currentJob == NULL || scheduleChanged) {
      // have to get (possibly) new job
      if (mySchedule.size() > 0) {
	currentJob = mySchedule.front();
	if (prevJob != NULL && prevJob != currentJob) {
	  prevJob->preempt();
	}
      }
      else {
	currentJob = NULL;
      }
    }
    // now we know which job is to be executed next (->currentJob)

    if (currentJob == NULL) { // no job -> idle
      dispatchStat.idle = true;
      return NULL;
    }
    else {
      LOG(LOG_CLASS_SCHEDULER) << "\tExecuting job " << *currentJob;
      bool fin = currentJob->execStep(now);
      dispatchStat.executed = currentJob;
      if (fin) { // (currentJob->etRemain == 0) {
	Job* finishedJob = currentJob;
	assert(finishedJob == mySchedule.front());
	mySchedule.pop_front();
	if (finishedJob->getAbsDeadline() <= now) {
	  dispatchStat.dlMiss = true;
	}
	LOG(LOG_CLASS_SCHEDULER) << "Finished job " << *finishedJob << " @ "
				 << finishedJob << " finished.";
	dispatchStat.finished = finishedJob;
	jobFinished(finishedJob);
	currentJob = NULL;

	if (finishedJob->getAbsDeadline() <= now) {
	  // job has missed its deadline, search in execMissJobs list
	  list<const Job*>::iterator it = execMissJobs.begin();
	  while (it != execMissJobs.end()) {
	    if (*it == finishedJob) {
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
	    LOG(LOG_CLASS_SCHEDULER) << "Could not find DL-miss job "
				     << finishedJob << " (" << *finishedJob
				     << ") in execMissJobs list (now = "
				     << now << ")!";
	  }
	}
	else if (finishedJob != dlmon.removeJob(finishedJob)) {
	  LOG(LOG_CLASS_SCHEDULER) << "Finished job " << finishedJob << " "
				   << *finishedJob
				   << " not found in dlmon (now = "
				   << now << ")!";
	}
	else {
	  LOG(LOG_CLASS_SCHEDULER) << "Removed finished job " << finishedJob
				   << " " << *finishedJob << " from dlmon!";
	}
	return finishedJob;
      }
      else {
	if (dlmon.jobExecuted(currentJob) != currentJob) {
	  //tError() << "Notifying dlmon failed! " << exc << " " << *exc;
	  // Don't care about this, the job should be in the execMissJobs list
	}
	return NULL;
      }
    }
  }


  bool ALDScheduler::hasPendingJobs(void) const {
    return mySchedule.size() > 0;
  }


  void ALDScheduler::printSchedule() const {
    for (list<Job*>::const_iterator it = mySchedule.begin();
	 it != mySchedule.end(); ++it) {
      tDebug() << "\t" << **it;
    }
  }


  void ALDScheduler::jobFinished(__attribute__((unused)) Job *job) {
  }


  void ALDScheduler::jobEnqueued(Job *job) {
    dlmon.addJob(job);
  }


  void ALDScheduler::notifyScheduleChanged() {
    scheduleChanged = true;
  }

} // NS tmssim
