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
 * $Id: uamean.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file uamean.cpp
 * @brief Implementation of aggregator with equal weight
 * @authors Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

#include <utility/uamean.h>

namespace tmssim {

  static const string ELEM_NAME = "uawmean";


  UAMean::UAMean() {
  }
  
  
  UAMean::~UAMean(void) {
  }
  
  
  double UAMean::getCurrentUtility(void) const {
    return getMeanUtility();
  }
  
  
  double UAMean::predictUtility(double u) const {
    double nextU = getTotal() + u;
    int nextC = getCount() + 1;
    return nextU / nextC;
  }
  
  
  void UAMean::addHook(__attribute__((unused)) double u) {
    // do nothing
  }
  
  void UAMean::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer,(xmlChar*) "uamean");
    xmlTextWriterEndElement(writer);
  }
  
  UtilityAggregator* UAMean::clone() const {
    UtilityAggregator* ua = new UAMean();
    return ua;
  }
  

  const std::string& UAMean::getClassElement() {
    return ELEM_NAME;
  }


  int UAMean::writeData(xmlTextWriterPtr writer) {
    return UtilityAggregator::writeData(writer);
  }


} // NS tmssim

