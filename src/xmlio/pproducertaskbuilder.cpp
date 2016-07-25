/**
 * $Id: pproducertaskbuilder.cpp 1422 2016-06-22 08:59:13Z klugeflo $
 * @file pproducertaskbuilder.cpp
 * @brief Building of pproducer tasks from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/pproducertaskbuilder.h>

#include <string>
#include <sstream>

#include <utils/tlogger.h>
#include <xmlio/xmlutils.h>
#include <xmlio/utilitycalculatorfactory.h>
#include <xmlio/utilityaggregatorfactory.h>
#include <tseval/pproducertask.h>

using namespace std;

namespace tmssim {

  Task* PProducerTaskBuilder::build(xmlDocPtr doc, xmlNodePtr cur) {
    tDebug() << "Building a PProducerTask";
    
    // Factories for more complex objects
    UtilityCalculatorFactory ucfact;
    UtilityAggregatorFactory uafact;
    
    // Variables need to build the task
    unsigned int id = 0;
    int period = -1;
    int executiontime = -1;
    //int criticaltime = -1;
    //UtilityCalculator* uc = NULL;
    //UtilityAggregator* ua = NULL;
    //int offset = -1;
    int priority = 1;
    int bufferSize = 0;
    int responseTime = 0;
    
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
	/*} else if (XmlUtils::isNodeMatching(cur, "criticaltime")) {
	  criticaltime = XmlUtils::getNodeValue<int>(cur, doc);*/
      } else if (XmlUtils::isNodeMatching(cur, "period")) {
	period = XmlUtils::getNodeValue<int>(cur, doc);
	/*} else if (XmlUtils::isNodeMatching(cur, "offset")) {
	  offset = XmlUtils::getNodeValue<int>(cur, doc);*/
      } else if(XmlUtils::isNodeMatching(cur,"priority")) {
	priority = XmlUtils::getNodeValue<int>(cur,doc);
      } else if(XmlUtils::isNodeMatching(cur,"buffersize")) {
	bufferSize = XmlUtils::getNodeValue<int>(cur,doc);
      } else if(XmlUtils::isNodeMatching(cur,"responsetime")) {
	responseTime = XmlUtils::getNodeValue<int>(cur,doc);
      } else {
	//tError() << "Unknown Node Tag: " << cur->name;
      }
      
      cur = cur->next;
    }
    // Create the entire Task object from the parsed data
    PProducerTask* taskPtr = new PProducerTask(id, period, executiontime, priority, bufferSize, responseTime, NULL);

    return taskPtr;
  }
  
} // NS tmssim
