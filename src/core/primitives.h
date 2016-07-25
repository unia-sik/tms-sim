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
 * $Id: primitives.h 1407 2016-05-31 07:30:51Z klugeflo $
 * @file primitives.h
 * @brief Primitive data types
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <cstdint>

namespace tmssim {

  /**
   * Time counter
   */
  typedef int64_t TmsTime;

  /**
   * Description of time intervals, e.g. periods, execution times
   */
  typedef unsigned int TmsTimeInterval;
  #define TMS_TIME_INTERVAL_MIN 0
  #define TMS_TIME_INTERVAL_MAX UINT_MAX

  /**
   * Task/Job Priority
   */
  typedef unsigned int TmsPriority;
#define TMS_MAX_PRIORITY ((TmsPriority) 0)
#define TMS_MIN_PRIORITY ((TmsPriority) UINT_MAX)

  
} // NS tmssim
