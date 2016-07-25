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
 * $Id: deadlinemonitor.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file deadlinemonitor.cpp
 * @brief Implementation of Deadline Monitor
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <core/deadlinemonitor.h>
#include <utils/tlogger.h>

using namespace std;

namespace tmssim {

  DeadlineMonitor::DeadlineMonitor() {
  }


  DeadlineMonitor::~DeadlineMonitor() {
  }

  
  void DeadlineMonitor::addJob(const Job* job) {
    if (job == NULL) {
      // might also throw some error
      return;
    }
    list<const Job*>::iterator it = jobs.begin();
    while ( it != jobs.end()
	    && *it != NULL
	    && (*it)->getLatestStartTime() <= job->getLatestStartTime()) {
      it++;
    }
    jobs.insert(it, job);
  }

  
  const Job* DeadlineMonitor::check(TmsTime now) {
    if (jobs.size() == 0)
      return NULL;
    const Job* job = jobs.front();
    if (job->getLatestStartTime() < now) {
      jobs.pop_front();
      return job;
    }
    else {
      return NULL;
    }
  }


  const Job* DeadlineMonitor::jobExecuted(const Job* job) {
    //cout << "Recording execution of job " << *job << endl;
    list<const Job*>::iterator it = jobs.begin();
    while ( it != jobs.end() && *it != job ) {
      it++;
    }
    if (it == jobs.end())
      return NULL; // finished job not found

    // job execution can only increase the latest start time, so we need only
    // to look at jobs that are at the back of the list
    list<const Job*>::iterator itJob = it;
    it++; // goto next

    if ( it == jobs.end() || (*it)->getLatestStartTime() >= job->getLatestStartTime() ) {
      return job;
    }
    // else we need to reorder...
    it = jobs.erase(itJob);
    while ( it != jobs.end() && *it != NULL
	    && (*it)->getLatestStartTime() < job->getLatestStartTime() ) {
      it++;
    }
    jobs.insert(it, job);
    return job;
  }


  const Job* DeadlineMonitor::removeJob(const Job* job) {
    list<const Job*>::iterator it = jobs.begin();
    while ( it != jobs.end() && *it != job ) {
      it++;
    }
    if (it == jobs.end())
      return NULL;
    jobs.erase(it);
    //tDebug() << "DLMon removed job " << job << " " << *job;
    return job;
  }

  

} // NS tmssim
