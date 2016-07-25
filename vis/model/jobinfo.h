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
 */

/**
 * $Id: jobinfo.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file jobinfo.h
 * @brief Trace data prepared for display
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef JOBINFO_H
#define JOBINFO_H

#include <list>
#include <map>
#include <ostream>
#include <string>

/**
 * Store information about a job's current execution state and events
 */
struct JobInfo {
  /**
   * @brief What can happen to a task/job.
   * Use these constants to index #JobInfo::event.
   * @attention If you add an event to this list, do so before the
   * #JobInfo::Event::LAST entry to ensure correct size of the
   * #JobInfo::event array.
   */
  enum Event {
    ACTIVATE = 0, ///< Job activation
    START, ///< Job execution starts
    PREEMPT, ///< Job execution preempted
    RESUME, ///< Job execution resumed
    FINISH, ///< Job execution finished
    CANCEL, ///< Job cancelled

    LAST
  };

  /// What can the job be doing
  enum ExecState {
    NONE = 0,
    READY,
    EXECUTE
  };

  /**
   * @todo Remove events, it might be better to set them explicitly
   */
  JobInfo(unsigned long _time, unsigned _jobId, ExecState _execState, std::string _taskState);/*,
	  bool eActivate = false,
	  bool ePreempt = false,
	  bool eResume = false,
	  bool eFinish = false,
	  bool eCancel = false);*/


  void addEvent(Event e);
  void removeEvent(Event e);
  bool getEvent(Event e) const;

  /// When did the event happen
  unsigned long time;
  /// JobId
  unsigned jobId;
  /// What did happen
  bool event[JobInfo::LAST];
  /// new state of the job due to the events
  ExecState execState;
  
  std::string taskState;
};

std::ostream& operator << (std::ostream& ost, JobInfo ji);

/**
 * @brief Execution trace of a task.
 *
 * Structure of this list:
 * - all entries are ordered with increasing time
 * - there is at most one entry for each time step
 * - Events occurring the same time step are recorded in a single #JobInfo
 * - only state changes are recorded, during times between two #JobInfo-s,
 *   the job retains the state of the first #JobInfo.
 * - All events relate to the beginning of the time step
 * - i.e. for execution: if a job finishes execution, the finish event is
 *   recorded with a JobInfo having state #JobInfo::State::NONE and the
 *   #JobInfo::Event::FINISH event set.
 * - There can be at most one job active at any time (job execution must be
 *   finished before next activation, else job has to be cancelled)
 *
 * For convenience, this class takes ownership of the memory of its contents.
 * It overwrites the std::list destructor to automatically free all memory
 * occupied by its contents.
 */
class JobInfoList: public std::list<JobInfo*> {
 public:
  ~JobInfoList();
};


/**
 * @brief For convenience of memory management.
 */
class TraceMap: public std::map<std::string, JobInfoList*> {
 public:
  ~TraceMap();
};



#endif // !JOBINFO_H
