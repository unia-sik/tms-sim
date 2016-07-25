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
 * $Id: edf.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file edf.cpp
 * @brief Implementation of EDF scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <schedulers/edf.h>

#include <cassert>
#include <climits>
#include <cstddef>
#include <list>

#include <iostream>


using namespace std;

namespace tmssim {

  static const std::string myId = "EDFScheduler";
  
  
  EDFScheduler::EDFScheduler(const SchedulerConfiguration& schedulerConfiguration)
    : ALDScheduler(schedulerConfiguration) {
  }
  
  
  EDFScheduler::~EDFScheduler() {
  }
  

  void EDFScheduler::enqueueJob(Job *job) {
    assert(job->getTask() != NULL);
    assert((long long)job->getTask() < 0x800000000000LL); // WTF???
    std::list<Job*>::iterator it = mySchedule.begin();
    while ( it != mySchedule.end()
	    && *it != NULL
	    && (*it)->getAbsDeadline() <= job->getAbsDeadline() ) {
      it++;
    }
    mySchedule.insert(it, job);
    notifyScheduleChanged();
    jobEnqueued(job);
    //return true;
  }


  int EDFScheduler::schedule(__attribute__((unused)) TmsTime now,
			     __attribute__((unused)) ScheduleStat& scheduleStat) {
    // Deadline monitoring is done by ALDScheduler,
    // which is better because it checks the whole list!
    /*
    if (mySchedule.size() == 0)
      return 0;
    bool firstJobValid = false;
    while (!firstJobValid && mySchedule.size() != 0) {
      Job* job = mySchedule.front();
      if (job->getLatestStartTime() < now) {
	scheduleStat.cancelled.push_back(job);
	mySchedule.pop_front();
      }
      else {
	firstJobValid = true;
      }
    }
    */
    return 0;
  }


  const std::string& EDFScheduler::getId(void) const {
    return myId;
  }


  const Job* EDFScheduler::checkEDFSchedule(TmsTime now) const {
    TmsTime time = now;
    for (list<Job*>::const_iterator it = mySchedule.begin();
	 it != mySchedule.end(); ++it) {
      time += (*it)->getRemainingExecutionTime();
      if (time > (*it)->getAbsDeadline()) {
	return *it;
      }
    }
    return NULL;
  }


  const Job* EDFScheduler::checkEDFSchedule(TmsTime now, const list<Job*>& schedule) {
    TmsTime time = now;
    for (list<Job*>::const_iterator it = schedule.begin();
	 it != schedule.end(); ++it) {
      time += (*it)->getRemainingExecutionTime();
      if (time > (*it)->getAbsDeadline()) {
	return *it;
      }
    }
    return NULL;
  }
  
  /*
  std::ostream& operator << (std::ostream& ost, const EDFScheduler& scheduler) {
    scheduler.printSchedule(ost);
    return ost;
  }
  */


  Scheduler* EDFSchedulerAllocator(const SchedulerConfiguration& schedulerConfiguration) {
    return new EDFScheduler(schedulerConfiguration);
  }

} // NS tmssim
