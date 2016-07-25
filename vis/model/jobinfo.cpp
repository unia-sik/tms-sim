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
 * $Id: jobinfo.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file jobinfo.cpp
 * @brief Trace data prepared for display
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include "jobinfo.h"

using namespace std;

JobInfo::JobInfo(unsigned long _time, unsigned _jobId, ExecState _execState, std::string _taskState)/*,
		 bool eActivate,
		 bool ePreempt,
		 bool eResume,
		 bool eFinish,
		 bool eCancel)*/
  : time(_time), jobId(_jobId), execState(_execState), taskState(_taskState) {
  for (size_t i = 0; i < JobInfo::LAST; ++i) {
    event[i] = false;
  }
  /*
  event[ACTIVATE] = eActivate;
  event[PREEMPT] = ePreempt;
  event[RESUME] = eResume;
  event[FINISH] = eFinish;
  event[CANCEL] = eCancel;
  */
}


void JobInfo::addEvent(Event e) {
  if (e >= LAST)
    return;
  else
    event[e] = true;
}


void JobInfo::removeEvent(Event e) {
  if (e >= LAST)
    return;
  else
    event[e] = false;
}


bool JobInfo::getEvent(Event e) const {
  if (e >= LAST)
    return false; // maybe throw exception
  else
    return event[e];
}


#define BOOL2B(b) ((b)?1:0)

std::ostream& operator << (std::ostream& ost, JobInfo ji) {
  ost << "{" << ji.jobId << "@" << ji.time
      << " ";
  for (size_t i = 0; i < JobInfo::LAST; ++i) {
    ost << BOOL2B(ji.event[i]);
  }
  /*
      << BOOL2B(ji.event[0])
      << BOOL2B(ji.event[1])
      << BOOL2B(ji.event[2])
      << BOOL2B(ji.event[3])
      << BOOL2B(ji.event[4])
  */
  ost << " "
      << ji.execState
      << " \""
      << ji.taskState
      << "\"}";
  return ost;
}


JobInfoList::~JobInfoList() {
  for(JobInfo* ji: *this) {
    delete ji;
  }
}


TraceMap::~TraceMap() {
  for(pair<std::string, JobInfoList*> p: *this) {
    delete p.second;
  }
}


