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
 * $Id: sporadictaskbuilder.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file sporadictaskbuilder.cpp
 * @brief Building of sporadic tasks from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/sporadictaskbuilder.h>

#include <string>
#include <sstream>

#include <utils/tlogger.h>
#include <xmlio/xmlutils.h>
#include <xmlio/utilitycalculatorfactory.h>
#include <xmlio/utilityaggregatorfactory.h>
#include <taskmodels/sporadictask.h>

using namespace std;

namespace tmssim {
  
  Task* SporadicTaskBuilder::build(xmlDocPtr doc, xmlNodePtr cur) {
    
    tDebug() << "Building a SporadicTask";
    
    // Factories for more complex objects
    UtilityCalculatorFactory ucfact;
    UtilityAggregatorFactory uafact;
    
    // Variables need to build the task
    unsigned int id = 0;
    int minPeriod = -1;
    int executiontime = -1;
    int criticaltime = -1;
    UtilityCalculator* uc = NULL;
    UtilityAggregator* ua = NULL;
    unsigned int seed = 0;
    double probability = 0.0;
    int offset = -1;
    int priority = 1;
    
    // Iterate through the document to get the needed data
    cur = cur->xmlChildrenNode;
    // Iterate over all children elements like <id>, <executiontime>, ...
    while (cur != NULL) {
      
      if (ucfact.accept(doc, cur)) {
	uc = ucfact.getElement(doc, cur);
      } else if (uafact.accept(doc, cur)) {
	ua = uafact.getElement(doc, cur);
      }
      if (XmlUtils::isNodeMatching(cur, "id")) {
	id = XmlUtils::getNodeValue<unsigned int>(cur, doc);
      } else if (XmlUtils::isNodeMatching(cur, "executiontime")) {
	executiontime = XmlUtils::getNodeValue<int>(cur, doc);
      } else if (XmlUtils::isNodeMatching(cur, "criticaltime")) {
	criticaltime = XmlUtils::getNodeValue<int>(cur, doc);
      } else if (XmlUtils::isNodeMatching(cur, "minperiod")) {
	minPeriod = XmlUtils::getNodeValue<int>(cur, doc);
      } else if (XmlUtils::isNodeMatching(cur, "seed")) {
	seed = XmlUtils::getNodeValue<unsigned int>(cur, doc);
      } else if (XmlUtils::isNodeMatching(cur, "probability")) {
	probability = XmlUtils::getNodeValue<double>(cur, doc);
      } else if (XmlUtils::isNodeMatching(cur, "offset")) {
	offset = XmlUtils::getNodeValue<int>(cur, doc);
      } else if(XmlUtils::isNodeMatching(cur,"priority")) {
	priority = XmlUtils::getNodeValue<int>(cur,doc);
      } else {
	//tError() << "Unknown Node Tag: " << cur->name;
      }
      
      cur = cur->next;
    }
    
    // Create the entire task from the parsed data
    SporadicTask* taskPtr = new SporadicTask(id, minPeriod, executiontime, seed, criticaltime, uc, ua, probability, offset, priority);
	return taskPtr;
  }
  
} // NS tmssim
