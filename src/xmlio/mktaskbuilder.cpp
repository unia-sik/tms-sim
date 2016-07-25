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
 * $Id: mktaskbuilder.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file mktaskbuilder.cpp
 * @brief Build tmssim::MkTask from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/mktaskbuilder.h>

#include <string>
#include <sstream>
#include <utils/tlogger.h>
#include <xmlio/xmlutils.h>
#include <xmlio/utilitycalculatorfactory.h>
#include <xmlio/utilityaggregatorfactory.h>
#include <taskmodels/mktask.h>

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost::algorithm;

namespace tmssim {

  Task* MkTaskBuilder::build(xmlDocPtr doc, xmlNodePtr cur) {
    
    tDebug() << "Building a MkTask";
    
    // Factories for more complex objects
    UtilityCalculatorFactory ucfact;
    UtilityAggregatorFactory uafact;
    
    // Variables need to build the task
    unsigned int id = 0;
    int period = -1;
    int executiontime = -1;
    int criticaltime = -1;
    int priority = 1;
    int m = 0;
    int k = 0;
    unsigned int spin = 0;
    UtilityCalculator* uc = NULL;
    UtilityAggregator* ua = NULL;
    string strMkState = "";

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
      } else if (XmlUtils::isNodeMatching(cur, "period")) {
	period = XmlUtils::getNodeValue<int>(cur, doc);
      } else if (XmlUtils::isNodeMatching(cur, "priority")) {
	priority = XmlUtils::getNodeValue<int>(cur, doc);
      } else if (XmlUtils::isNodeMatching(cur, "m")) {
	m = XmlUtils::getNodeValue<int>(cur, doc);
      } else if (XmlUtils::isNodeMatching(cur, "k")) {
	k = XmlUtils::getNodeValue<int>(cur, doc);
      } else if (XmlUtils::isNodeMatching(cur, "spin")) {
	spin = XmlUtils::getNodeValue<int>(cur, doc);
      } else if (XmlUtils::isNodeMatching(cur, "mkstate")) {
	//strMkState = XmlUtils::getNodeValue<string>(cur, doc);
	strMkState = (char*)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
      } else {
	//tError() << "Unknown Node Tag: " << cur->name;
      }
      // FIXME: uc, ua
      cur = cur->next;
    }

    // TODO: parse mkstate!
    CompressedMkState cMkState = CMKS_ALL_SUCCESS;
    //cout << "(m,k)-state: \"" << strMkState << "\"" << " spin: " << spin << endl;
    if (strMkState != "") {
      // trim
      trim_right(strMkState);
      trim(strMkState);
      //cout << "Trimmed (m,k)-state: \"" << strMkState << "\"" << endl;
      // parse
      //size_t len = strMkState.length();
      //cout << "nstates: " << len << endl;
      const char* cs = strMkState.c_str();
      cMkState = 0;
      //size_t i;
      while (*cs) {
	cMkState <<= 1;
	if (*cs == '1') cMkState |= 1;
	++cs;
      }
      //cout << "Number: " << cMkState << endl;
    }
    
    // Create the entire task from the parsed data

    MkTask* taskPtr = new MkTask(id, period, executiontime, criticaltime,
				 uc, ua, priority, m, k, spin, cMkState);

      /*
  MkTask::MkTask(unsigned int _id, int _period, int _et, int _ct,
	    UtilityCalculator* _uc, UtilityAggregator* _ua,
	    unsigned int _prio, unsigned int _m, unsigned int _k)
  */
    return taskPtr;
  }
  
} // NS tmssim
