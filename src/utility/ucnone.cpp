/**
 * $Id: ucnone.cpp 1407 2016-05-31 07:30:51Z klugeflo $
 * @file ucnone.cpp
 * @brief An empty implementation of a utility calculator
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <utility/ucnone.h>

#include <utils/tmsexception.h>

#include <core/globals.h>

namespace tmssim {

  static const string ELEM_NAME = "ucnone";

  double UCNone::calcUtility(UNUSED const Job *job, UNUSED TmsTime complTime) const {
    return 0;
  }

  
  UtilityCalculator* UCNone::clone() const {
    THROW_UNIMPLEMENTED;
  }


  int UCNone::writeData(xmlTextWriterPtr writer) {
    UtilityCalculator::writeData(writer);
    return 0;
  }
  

  const std::string& UCNone::getClassElement() {
    return ELEM_NAME;
  }
  
  
} // NS tmssim
