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
 * $Id: uawmeanbuilder.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file uawmeanbuilder.cpp
 * @brief Build tmssim::UAWMean from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/uawmeanbuilder.h>

#include <string>
#include <sstream>
#include <iostream>

#include <utils/tlogger.h>
#include <xmlio/xmlutils.h>
#include <utility/uawmean.h>

using namespace std;

namespace tmssim {

  UtilityAggregator* UAWMeanBuilder::build(xmlDocPtr doc, xmlNodePtr cur) {
    tDebug() << "Building a UAWMean";
    
    // Variables need to build the task
    size_t size = 0;
    
    // Iterate through the document to get the needed data
    cur = cur->xmlChildrenNode;
    // Iterate over all children elements
    while (cur != NULL) {
      
      if (XmlUtils::isNodeMatching(cur, "size")) {
	size = XmlUtils::getNodeValue<size_t>(cur, doc);
      } else {
	//tError() << "Unknown Node Tag: " << cur->name;
      }
      
      cur = cur->next;
    }
    // Create the entire UtilityAggregator from the parsed data
    tDebug() << "size=" << size;
    
    UAWindow* uaptr = new UAWMean(size);
    
    return uaptr;
  }
  
} // NS tmssim
