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
 * $Id: utilityaggregator.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file utilityaggregator.cpp
 * @brief Aggregation of job utilities into a task utility
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <core/utilityaggregator.h>
//#include <core/scobjects.h>

#include <cmath>
#include <iostream>

namespace tmssim {

  UtilityAggregator::UtilityAggregator()
    : utilitySum(0), utilityCount(0) {
  }
  
  
  UtilityAggregator::~UtilityAggregator(void) {
  }
  
  
  void UtilityAggregator::addUtility(double u) {
    if (std::isnan(u) || std::isinf(u)) {
      // we have a problem!
      // TODO: solve!
    }
    else {
      utilitySum += u;
      ++utilityCount;
    }
    addHook(u);
  }
  
  
  int UtilityAggregator::getCount(void) const {
    return utilityCount;
  }
  
  
  double UtilityAggregator::getTotal(void) const {
    return utilitySum;
  }
  
  
  double UtilityAggregator::getMeanUtility(void) const {
    return utilitySum/utilityCount;
  }
  
  
  std::ostream& operator << (std::ostream& ost, const UtilityAggregator& ua) {
    ost << "[" << ua.utilitySum << "/" << ua.utilityCount 
	<< "=" << ua.getMeanUtility() << "]";
    return ost;
  }

  
  /// @todo throw exception?
  int UtilityAggregator::writeData(__attribute__((unused)) xmlTextWriterPtr writer) {
    return 0;
  }
  
} // NS tmssim
