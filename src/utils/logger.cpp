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
 * $Id: logger.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file logger.cpp
 * @brief Simple logging facility
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <utils/logger.h>
#include <utils/nullstream.h>

//#include <iostream>
//#include <cstdlib>

using namespace std;

namespace tmssim {
  LogClass detail::llogger::globalClass = 0;

  
  map<std::string, LogClass>* detail::llogger::nameMapping = NULL;


  const string detail::LOG_PREFIX[] = {
    "TASK",
    "JOB",
    "SCHED",
    "EXEC",
    "SIM",
    "RESERVED05",
    "RESERVED06",
    "RESERVED07",
    "RESERVED08",
    "RESERVED09",
    "RESERVED10",
    "RESERVED11",
    "RESERVED12",
    "RESERVED13",
    "RESERVED14",
    "RESERVED15",
    "EVAL_MK",
    "EVAL_TS",
    "RESERVED18",
    "RESERVED19",
    "RESERVED20",
    "RESERVED21",
    "RESERVED22",
    "RESERVED23",
    "RESERVED24",
    "RESERVED25",
    "RESERVED26",
    "RESERVED27",
    "RESERVED28",
    "RESERVED29",
    "RESERVED30",
    "RESERVED31"
  };


  
  const uint32_t DEBRUIJN32 = 0x76BE629UL;

  static const uint32_t DEBRUIJN32_TABLE[] = {
    0,  1, 23,  2, 29, 24, 19,  3,
    30, 27, 25, 11, 20,  8,  4, 13,
    31, 22, 28, 18, 26, 10,  7, 12,
    21, 17,  9,  6, 16,  5, 15, 14
  };

  size_t detail::bit2offset(uint32_t _bit) {
    // plucked from http://blog.incubaid.com/tag/de-bruijn/
    uint32_t bit = _bit & -_bit;
    bit *= DEBRUIJN32;
    bit >>= 27;
    return DEBRUIJN32_TABLE[bit];
  }

  
  void detail::llogger::activateClass(std::string cls) {
    try {
      LogClass c = getNameMapping()->at(cls);
      globalClass |= c;
    }
    catch (out_of_range& e) {
      cerr << "Invalid log class " << cls << endl;
      throw e;
    }
  }

  
  void detail::llogger::deactivateClass(std::string cls) {
    LogClass c = getNameMapping()->at(cls);
    globalClass &= ~c;
  }
  
  
  void detail::llogger::initNameMapping() {
    static detail::llogger::Guard guard;
    if (nameMapping != NULL) {
      return;
    }
    nameMapping = new map<std::string, LogClass>;
    for (size_t i = 0; i < LOG_N_PREFIX; ++i) {
      (*nameMapping)[detail::LOG_PREFIX[i]] = 1 << i;
      //cout << detail::LOG_PREFIX[i] << " : " << hex << (*nameMapping)[detail::LOG_PREFIX[i]] << dec << endl;
    }
  }

    
} // NS tmssim
