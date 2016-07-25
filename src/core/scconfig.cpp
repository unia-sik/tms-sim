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
 * $Id: scconfig.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file scconfig.cpp
 * @brief Configuration parameters for schedulers
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <core/scconfig.h>

namespace tmssim {

  const bool SchedulerConfiguration::defaultExecCancellations = true;
  const bool SchedulerConfiguration::defaultDlMissCancellations = true;

  
  SchedulerConfiguration::SchedulerConfiguration(bool _execCancellations,
						 bool _dlMissCancellations)
    : execCancellations(_execCancellations),
      dlMissCancellations(_dlMissCancellations) {
  }

  
  SchedulerConfiguration::SchedulerConfiguration(const KvFile& conf) {
    if (conf.containsKey("execCancellations")) {
      execCancellations = conf.getBool("execCancellations");
    }
    else {
      execCancellations = SchedulerConfiguration::defaultExecCancellations;
    }

    if (conf.containsKey("dlMissCancellations")) {
      dlMissCancellations = conf.getBool("dlMissCancellations");
    }
    else {
      dlMissCancellations = SchedulerConfiguration::defaultDlMissCancellations;
    }
  }


  SchedulerConfiguration::SchedulerConfiguration(const KvFile* conf) {
    if (conf->containsKey("execCancellations")) {
      execCancellations = conf->getBool("execCancellations");
    }
    else {
      execCancellations = SchedulerConfiguration::defaultExecCancellations;
    }

    if (conf->containsKey("dlMissCancellations")) {
      dlMissCancellations = conf->getBool("dlMissCancellations");
    }
    else {
      dlMissCancellations = SchedulerConfiguration::defaultDlMissCancellations;
    }
  }

  
  SchedulerConfiguration DefaultSchedulerConfiguration; // = SchedulerConfiguration();


} // NS tmssim
