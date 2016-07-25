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
 * $Id: taskstatedecoder.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file taskstatedecoder.cpp
 * @brief Decode task state for display
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include "taskstatedecoder.h"

#include <boost/regex.hpp>

#include <iostream>

using namespace std;
using namespace boost;

// ==SIM== Task: TM0(7/2/7) [0;1] (3,4) s=0
const regex RE_TASK("^([A-Za-z]+[0-9]+)(.*)");
const regex RE_TYPE("^([A-Za-z]+)[0-9]+.*");



TaskStateDecoder::TaskStateDecoder(const string& taskDescription)
  : taskData(_taskData) {
  smatch sm;
  if (regex_match(taskDescription, sm, RE_TASK)) {
    _taskId = sm[1];
    _taskData = sm[2];
  }
  else {
    cerr << "Could not match task id in task tescription \""
	 << taskDescription << "\"" << endl;
  }
  if (regex_match(taskDescription, sm, RE_TYPE)) {
    _typeId = sm[1];
  }
  else {
    cerr << "Could not match/extract task type id from \""
	 << taskDescription << "\"" << endl;
  }
}


TaskStateDecoder::TaskStateDecoder(const TaskStateDecoder& rhs)
  : taskData(_taskData), _taskId(rhs._taskId), _typeId(rhs._typeId), _taskData(rhs._taskData)
{
}


TaskStateDecoder::~TaskStateDecoder() {
}


const std::string& TaskStateDecoder::getTaskId() const {
  return _taskId;
}


const std::string& TaskStateDecoder::getTypeId() const {
  return _typeId;
}


DecoderMap::~DecoderMap() {
  for (pair<const std::string, TaskStateDecoder*> p: *this) {
    delete p.second;
  }
}

