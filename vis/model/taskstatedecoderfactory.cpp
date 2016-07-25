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
 * $Id: taskstatedecoderfactory.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file taskstatedecoderfactory.cpp
 * @brief Create task state decoders
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include "taskstatedecoderfactory.h"

#include "defaulttaskstatedecoder.h"
#include "mktaskstatedecoder.h"

#include <boost/regex.hpp>

#include <iostream>

using namespace std;
using namespace boost;

// ==SIM== Task: TM0(7/2/7) [0;1] (3,4) s=0
// we're only interested in the task type, but let's match the first digit
// as well to be sure
const regex RE_TASK_TYPE("^([A-Za-z]+)[0-9].*");

TaskStateDecoder* TaskStateDecoderFactory::getDecoder(const string& taskDescription) {
  smatch sm;
  if (regex_match(taskDescription, sm, RE_TASK_TYPE)) {
    const string& type = sm[1];
    //cout << "Task type: " << type << endl;
    // first char should be 'T', ignore this
    switch (type[1]) {
    case 'M':
      //cout << "Building MkTaskStateDecoder" << endl;
      return new MkTaskStateDecoder(taskDescription);

    default:
      //cout << "Unknown task type, using DefaultTaskStateDecoder" << endl;
      return new DefaultTaskStateDecoder(taskDescription);
    }
  }
  else {
    cerr << "Could not match task type in \"" << taskDescription << "\"" << endl;
    return NULL;
  }
}
