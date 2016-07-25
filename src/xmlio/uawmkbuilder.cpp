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
 * $Id: uawmkbuilder.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file uawmkbuilder.cpp
 * @brief Build tmssim::UAWMK from XML
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/uawmkbuilder.h>

#include <string>
#include <sstream>
#include <iostream>

#include <utils/tlogger.h>
#include <xmlio/xmlutils.h>
#include <utility/uawmk.h>

using namespace std;

namespace tmssim {

  UtilityAggregator* UAWMKBuilder::build(xmlDocPtr doc, xmlNodePtr cur) {
    tDebug() << "Building a UAWMK";
    
    // Variables need to build the UtilityAggregator
    unsigned int m = 0;
    unsigned int k = 0;
    
    // Iterate through the document to get the needed data
    cur = cur->xmlChildrenNode;
    // Iterate over all children elements
    while (cur != NULL) {
      
      if (XmlUtils::isNodeMatching(cur, "m")) {
	m = XmlUtils::getNodeValue<size_t>(cur, doc);
      }
      else if (XmlUtils::isNodeMatching(cur, "k")) {
	k = XmlUtils::getNodeValue<size_t>(cur, doc);
      }
      else {
	//tError() << "Unknown Node Tag: " << cur->name;
      }
      
      cur = cur->next;
    }
    return new UAWMK(m,k);
  }

} // NS tmssim
