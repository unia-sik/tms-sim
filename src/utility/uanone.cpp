/**
 * $Id: uanone.cpp 1407 2016-05-31 07:30:51Z klugeflo $
 * @file uanone.cpp
 * @brief An empty implementation of a utility aggregator
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <utility/uanone.h>

#include <utils/tmsexception.h>

#include <core/globals.h>

namespace tmssim {


  static const string ELEM_NAME = "uanone";


  UANone::UANone()
    : UtilityAggregator()
  {
  }

  
  double UANone::getCurrentUtility(void) const {
    return 0;
  }

  
  double UANone::predictUtility(UNUSED double u) const {
    return 0;
  }
    

  UtilityAggregator* UANone::clone() const {
    THROW_UNIMPLEMENTED;
  }    

  
  int UANone::writeData(xmlTextWriterPtr writer) {
    UtilityAggregator::writeData(writer);
    return 0;
  }

  
  const std::string& UANone::getClassElement() {
    return ELEM_NAME;
  }

  
  void UANone::addHook(UNUSED double u) {
  }
  

} // NS tmssim
