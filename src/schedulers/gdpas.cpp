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
 * $Id: gdpas.cpp 1421 2016-06-22 07:46:32Z klugeflo $
 * @file gdpas.cpp
 * @brief Simplified Guaranteed Dynamic Priority Assigment Scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <schedulers/gdpas.h>

#include <cassert>

#include <schedulers/edf.h>
#include <utils/logger.h>
#include <utils/tlogger.h>


namespace tmssim {

  static const std::string myId = "GDPA-SScheduler";


  GDPASScheduler::GDPASScheduler(const SchedulerConfiguration& schedulerConfiguration)
    : Scheduler(), myConfig(schedulerConfiguration), /*readyListChanged(false),*/
      dispatchListsChanged(false), /*edfFeasible(false),*/ currentJob(NULL) {

  }


  GDPASScheduler::~GDPASScheduler() {
    list<Job*>::iterator it = readyList.begin();
    while (it != readyList.end()) {
      Job* job = *it;
      tWarn() << "ALDScheduler destroys unfinished job " << *job;
      //<< " in instance of " << getId();
      delete job;
      it = readyList.erase(it);
    }
  }


  void GDPASScheduler::enqueueJob(Job *job) {
    // lines 6-8
    assert(job != NULL);
    assert(job->getTask() != NULL);
    //LOG(-1) << "Enqueueing job " << job;
    readyList.push_back(job);
    //readyListChanged = true;

    int distance = job->getTask()->getDistance();
    list<Job*>::iterator insSDF = sdfList.begin();
    while ( insSDF != sdfList.end()
	    && *insSDF != NULL
	    && (*insSDF)->getTask()->getDistance() <= distance ) {
      insSDF++;
    }
    sdfList.insert(insSDF, job);

    //Job* job = *it;
    std::list<Job*>::iterator insEDF = edfList.begin();
    while ( insEDF != edfList.end()
	    && *insEDF != NULL
	    && (*insEDF)->getAbsDeadline() <= job->getAbsDeadline() ) {
      insEDF++;
    }
    edfList.insert(insEDF, job);
    dispatchListsChanged = true;
  }


  int GDPASScheduler::initStep(TmsTime now, ScheduleStat& scheduleStat) {
    // lines 14-18
    list<Job*>::iterator it = readyList.begin();
    while (it != readyList.end()) {
      LOG(LOG_CLASS_SCHEDULER) << "Checking job...";
      if ( !(*it)->isFeasible(now) //(*it)->getLatestStartTime() < now
	   && (myConfig.execCancellations
	       || (*it)->getRemainingExecutionTime() == (*it)->getExecutionTime()) ) {
	Job* job = *it;
	it = readyList.erase(it);
	removeJobFromList(edfList, job);
	removeJobFromList(sdfList, job);
	scheduleStat.cancelled.push_back(job);
	if (currentJob == job)
	  currentJob = NULL;
	dispatchListsChanged = true;
      }
      else {
	++it;
      }
    }
    return 0;
  }


  const Job* GDPASScheduler::removeJob(__attribute__((unused)) const Job *job) {
    return NULL;
  }


  bool GDPASScheduler::hasPendingJobs(void) const {
    return readyList.size() > 0;
  }


  int GDPASScheduler::schedule(__attribute__((unused)) TmsTime now, __attribute__((unused)) ScheduleStat& scheduleStat) {
    /*
    LOG(LOG_CLASS_SCHEDULER) << "Creating new schedule?";
    if (readyListChanged) {
      LOG(LOG_CLASS_SCHEDULER) << "\tYes! R: " << readyList.size()
	     << " E: " << edfList.size() << " S: " << sdfList.size();

      // lines 14-18
      list<Job*>::iterator it = readyList.begin();
      while (it != readyList.end()) {
	LOG(LOG_CLASS_SCHEDULER) << "Checking job...";
	if ( (*it)->getLatestStartTime() < now
	     && (myConfig.execCancellations
		 || (*it)->getRemainingExecutionTime() == (*it)->getExecutionTime()) ) {
	  Job* job = *it;
	  it = readyList.erase(it);
	  removeJobFromList(edfList, job);
	  removeJobFromList(sdfList, job);
	  scheduleStat.cancelled.push_back(job);
	  dispatchListsChanged = true;
	}
	else {
	  ++it;
	}
      }

    }
    */
    return 0;
  }


  Job* GDPASScheduler::dispatch(TmsTime now, DispatchStat& dispatchStat) {
    Job* prevJob = currentJob;
    //bool oldDlc = dispatchListsChanged;
    bool edfFeasible = false;
    if (currentJob == NULL || dispatchListsChanged) {
      // lines 19-23
      edfFeasible = EDFScheduler::checkEDFSchedule(now, edfList) == NULL;
      //currentJob = edfFeasible ? edfList.front() : sdfList.front();
      if (readyList.size() > 0) {
	if (edfFeasible) {
	  currentJob = edfList.front();
	}
	else {
	  currentJob = sdfList.front();
	}
      }
      else {
	currentJob = NULL;
      }
      dispatchListsChanged = false;
    }
    if (prevJob != NULL && prevJob != currentJob) {
      prevJob->preempt();
    }
    if (currentJob == NULL) {
      dispatchStat.idle = true;
      return NULL;
    }

    tDebug() << "\tExecuting job " << *currentJob;
    bool fin = currentJob->execStep(now);
    dispatchStat.executed = currentJob;
    if (fin) { // (currentJob->etRemain == 0) {
      Job* finishedJob = currentJob;
      if (finishedJob->getAbsDeadline() <= now) {
	dispatchStat.dlMiss = true;
      }
      tDebug() << "Finished job " << *finishedJob << " @ " << finishedJob << " finished.";
      dispatchStat.finished = finishedJob;
      jobFinished(finishedJob);
      currentJob = NULL;
      return finishedJob;
    }
    else {
      return NULL;
    }
  }


  const std::string& GDPASScheduler::getId(void) const {
    return myId;
  }


  void GDPASScheduler::jobFinished(Job *job) {
    // lines 9-12
    removeJobFromList(readyList, job);
    removeJobFromList(edfList, job);
    removeJobFromList(sdfList, job);
    //readyListChanged = true;
    dispatchListsChanged = true;
    //feasibilityCheck();
  }


  void GDPASScheduler::feasibilityCheck(__attribute__((unused)) TmsTime now) {
    // lines 14-18
    /*
    list<Job*>::iterator it = readyList.begin();
    while (it != readyList.end()) {
      if ((*it)->getSt() < now) {
	Job* job = *it;
	it = readyList.erase(it);
	removeJobFromList(edfList, job);
	removeJobFromList(sdfList, job);
	queuesChanged = true;
      }
    }
    */
  }

  void GDPASScheduler::removeJobFromList(list<Job*>& jList, Job* job) {
    list<Job*>::iterator it = jList.begin();
    while (it != jList.end() && *it != job)
      it++;
    if (it != jList.end()) {
      jList.erase(it);
    }
  }


  Scheduler* GDPASSchedulerAllocator(const SchedulerConfiguration& schedulerConfiguration) { return new GDPASScheduler(schedulerConfiguration); }

} // NS tmssim
