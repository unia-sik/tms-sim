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
 * $Id: job.cpp 1407 2016-05-31 07:30:51Z klugeflo $
 * @file job.cpp
 * @brief Implementation of a TMS job
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

//#include <core/scobjects.h>
#include <core/job.h>
#include <core/task.h>

#include <cassert>
//#include <iostream>
#include <sstream>

#include <core/globals.h>

using namespace std;

namespace tmssim {

  Job::Job(Task* task, unsigned int jid, TmsTime _activationTime, TmsTimeInterval _executionTime, TmsTime _absDeadline, TmsPriority _priority)
    : myTask(task), jobId(jid), activationTime(_activationTime), executionTime(_executionTime), absDeadline(_absDeadline), priority(_priority),
      etRemain(_executionTime), preemptions(0)
  {
    assert(task != NULL);
    updateLatestStartTime();
  }
  
  
  Job::~Job() {
  }
  

  TmsTime Job::getActivationTime(void) const {
    return activationTime;
  }
  
  
  TmsTimeInterval Job::getExecutionTime(void) const {
    return executionTime;
  }
  
  
  TmsTime Job::getAbsDeadline(void) const {
    return absDeadline;
  }
  
  
  TmsTimeInterval Job::getRemainingExecutionTime(void) const {
    return etRemain;
  }
  
  
  Task* Job::getTask(void) const {
    return myTask;
  }

  
  TmsPriority Job::getPriority(){
    return priority;
  }

  
  bool Job::execStep(UNUSED TmsTime now) {
    etRemain--;
    updateLatestStartTime();
    if (etRemain <= 0) return true;
    else return false;
  }

  
  TmsTime Job::getLatestStartTime(void) const {
    return latestStartTime;
  }

  
  bool Job::isFeasible(TmsTime now) const {
    return (latestStartTime >= now);
  }


  void Job::updateLatestStartTime(void) {
    latestStartTime = absDeadline - etRemain;
  }


  void Job::preempt() {
    ++preemptions;
  }
  
  
  int Job::getPreemptions() const {
    return preemptions;
  }
  
  
  double Job::getPossibleExecValue(TmsTime startTime) const {
    return myTask->getPossibleExecValue(this, startTime);
  }
  
  
  double Job::getPossibleHistoryValue(TmsTime startTime) const {
    return myTask->getPossibleHistoryValue(this, startTime);
  }
  
  
  double Job::getPossibleFailHistoryValue() const {
    return myTask->getPossibleFailHistoryValue(this);
  }


  string Job::getIdString(void) const {
    ostringstream oss;
    /*
    unsigned int tid = myTask->getId();
    unsigned int jid = jobId;
    oss << "J" << tid << "," << jid;
    */
    oss << myTask->getIdString() << "," << jobId;
    return oss.str();
  }

  
  void Job::setPriority(TmsPriority p){
    priority = p;
  }
  

  ostream& operator << (std::ostream& ost, const Job& job) {
    ost << job.getIdString() << "(" << job.etRemain
	<< "/" << job.executionTime << "-" << job.absDeadline
	<< " P " << job.priority
	<< " S " << job.latestStartTime << ") " << job.myTask->strState();
    return ost;
  }
  
} // NS tmssim
