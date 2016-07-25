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
 * $Id: scconfig.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file scconfig.h
 * @brief Configuration parameters for schedulers
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef CORE_SCCONFIG_H
#define CORE_SCCONFIG_H

#include <utils/kvfile.h>

namespace tmssim {
  
  struct SchedulerConfiguration {

    static const bool defaultExecCancellations;
    static const bool defaultDlMissCancellations;
    
    SchedulerConfiguration(bool _execCancellations=defaultExecCancellations, bool _dlMissCancellations=defaultDlMissCancellations);
    SchedulerConfiguration(const KvFile& conf);
    SchedulerConfiguration(const KvFile* conf);

    bool execCancellations;
    bool dlMissCancellations;
  };

  extern SchedulerConfiguration DefaultSchedulerConfiguration;

} // NS tmssim

#endif // !CORE_SCCONFIG_H
