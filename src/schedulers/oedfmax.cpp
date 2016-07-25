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
 * $Id: oedfmax.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file oedfmax.cpp
 * @brief Implementation of overload EDF scheduler (maximum value removal)
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 * @todo Replace tDebug etc. by LOG(...)
 */

#include <cassert>
#include <cfloat>
#include <climits>
#include <cstddef>
#include <list>

#include <iostream>

#include <schedulers/oedfmax.h>

#include <utils/logger.h>

using namespace std;

namespace tmssim {

  static const std::string myId = "OEDFMaxScheduler";

  
  OEDFMaxScheduler::OEDFMaxScheduler(const SchedulerConfiguration& schedulerConfiguration)
    : OEDFScheduler(schedulerConfiguration) {
  }
  
  
  OEDFMaxScheduler::~OEDFMaxScheduler() {
  }


  const std::string& OEDFMaxScheduler::getId(void) const {
    return myId;
  }


  double OEDFMaxScheduler::getComparisonNeutral() const {
    return DBL_MIN;
  }

  
  bool OEDFMaxScheduler::compare(double current, double candidate) const {
    return candidate > current;
  }

  

} // NS tmssim
