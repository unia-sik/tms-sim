/**
 * $Id: pconsumertaskbuilder.cpp 1422 2016-06-22 08:59:13Z klugeflo $
 * @file pconsumertaskbuilder.cpp
 * @brief Building of pconsumer tasks from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/pconsumertaskbuilder.h>

#include <string>
#include <sstream>

#include <core/primitives.h>
#include <utils/tlogger.h>
#include <xmlio/xmlutils.h>
#include <xmlio/utilitycalculatorfactory.h>
#include <xmlio/utilityaggregatorfactory.h>
#include <tseval/tmppconsumertask.h>

using namespace std;

namespace tmssim {

  Task* PConsumerTaskBuilder::build(xmlDocPtr doc, xmlNodePtr cur) {
    tDebug() << "Building a PConsumerTask";
    
    // Factories for more complex objects
    UtilityCalculatorFactory ucfact;
    UtilityAggregatorFactory uafact;
    
    // Variables need to build the task
    unsigned int id = 0;
    TmsTimeInterval period = -1;
    TmsTimeInterval executiontime = -1;
    //UtilityCalculator* uc = NULL;
    //UtilityAggregator* ua = NULL;
    //int offset = -1;
    int priority = 1;
    TmsTimeInterval responseTime = 0;
    list<string>* producers = NULL;
    
    // Iterate through the document to get the needed data
    cur = cur->xmlChildrenNode;
    // Iterate over all children elements like <id>, <executiontime>, ...
    while (cur != NULL) {
      
      /*if (ucfact.accept(doc, cur)) {
	uc = ucfact.getElement(doc, cur);
      } else if (uafact.accept(doc, cur)) {
	ua = uafact.getElement(doc, cur);
	} else*/
      if (XmlUtils::isNodeMatching(cur, "id")) {
	id = XmlUtils::getNodeValue<unsigned int>(cur, doc);
      } else if (XmlUtils::isNodeMatching(cur, "executiontime")) {
	executiontime = XmlUtils::getNodeValue<int>(cur, doc);
      } else if (XmlUtils::isNodeMatching(cur, "period")) {
	period = XmlUtils::getNodeValue<int>(cur, doc);
	/*} else if (XmlUtils::isNodeMatching(cur, "offset")) {
	  offset = XmlUtils::getNodeValue<int>(cur, doc);*/
      } else if(XmlUtils::isNodeMatching(cur,"priority")) {
	priority = XmlUtils::getNodeValue<int>(cur,doc);
      } else if(XmlUtils::isNodeMatching(cur,"responsetime")) {
	responseTime = XmlUtils::getNodeValue<int>(cur,doc);
      } else if(XmlUtils::isNodeMatching(cur,"producers")) {
	producers = new list<string>;
	for (xmlNodePtr ch = cur->xmlChildrenNode; ch != NULL; ch = ch->next) {
	  if (XmlUtils::isNodeMatching(ch, "producer")) {
	    //tDebug() << "\tFound producers: " << ch->children->content;
	    producers->push_back((char*)ch->children->content);
	  }
	  else {
	    //tWarn() << "Unknown producers tag: " << ch->name;
	  }
	}
      } else {
	//tError() << "Unknown Node Tag: " << cur->name;
      }
      
      cur = cur->next;
    }
    // Create the entire Task object from the parsed data
    TmpPConsumerTask* taskPtr = new TmpPConsumerTask(id, period, executiontime, priority, responseTime, producers);

    return taskPtr;
  }
  
} // NS tmssim
