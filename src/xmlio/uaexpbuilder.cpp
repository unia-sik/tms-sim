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
 * $Id: uaexpbuilder.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file uaexpbuilder.cpp
 * @brief Build exponential utility aggregator from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/uaexpbuilder.h>

#include <string>
#include <sstream>
#include <iostream>

#include <utils/tlogger.h>
#include <xmlio/xmlutils.h>
#include <utility/uaexp.h>

using namespace std;

namespace tmssim {

  UtilityAggregator* UAExpBuilder::build(xmlDocPtr doc, xmlNodePtr cur) {
    tDebug() << "Building a UAExp";
    
    // Variables need to build the UtilityAggregator
    double weight = 0;
    
    // Iterate through the document to get the needed data
    cur = cur->xmlChildrenNode;
    // Iterate over all children elements
    while (cur != NULL) {
      
      if (XmlUtils::isNodeMatching(cur, "weight")) {
	weight = XmlUtils::getNodeValue<double>(cur, doc);
      } else {
	//tError() << "Unknown Node Tag: " << cur->name;
      }
      
      cur = cur->next;
    }
    // Create the entire UtilityAggregator from the parsed data
    tDebug() << "with weight=" << weight;
    
    UAExp* uaptr = new UAExp(weight);
    return uaptr;
    
  }
  
} // NS tmssim
