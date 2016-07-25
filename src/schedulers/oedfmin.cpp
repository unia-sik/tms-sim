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
 * $Id: oedfmin.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file oedfmin.cpp
 * @brief Implementaiton of EDF with cancellation, minimum value cancel policy
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <cfloat>
#include <climits>
#include <cstddef>
#include <list>

#include <iostream>

#include <schedulers/oedfmin.h>
#include <utils/logger.h>

using namespace std;

namespace tmssim {

  static const std::string myId = "OEDFMinScheduler";

  OEDFMinScheduler::OEDFMinScheduler(const SchedulerConfiguration& schedulerConfiguration)
    : OEDFScheduler(schedulerConfiguration) {
  }
  
  
  OEDFMinScheduler::~OEDFMinScheduler() {
  }
  
  
  const std::string& OEDFMinScheduler::getId(void) const {
    return myId;
  }


  double OEDFMinScheduler::getComparisonNeutral() const {
    return DBL_MAX;
  }
  
  bool OEDFMinScheduler::compare(double current, double candidate) const {
    return candidate < current;
  }

} // NS tmssim
