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
 * $Id: ucfirmrtbuilder.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file ucfirmrtbuilder.cpp
 * @brief Build firm RT UC from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/ucfirmrtbuilder.h>

#include <utils/tlogger.h>
#include <xmlio/xmlutils.h>
#include <utility/ucfirm.h>

namespace tmssim {
  
  UtilityCalculator* UCFirmRTBuilder::build(__attribute__((unused)) xmlDocPtr doc, __attribute__((unused)) xmlNodePtr cur) {
    tDebug() << "Building a UtilityCalculator";
    
    // No further information is needed to build the object
    return new UCFirmRT();
  }
  
} // NS tmssim
