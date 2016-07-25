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
 * $Id: uaexp.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file uaexp.cpp
 * @brief Implementation of aggregator with exponentially decreasing weight
 * @authors Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

#include <utility/uaexp.h>
#include <xmlio/xmlutils.h>

namespace tmssim {


  static const string ELEM_NAME = "uaexp";

  UAExp::UAExp(double _w)
    : weight(_w), currentValue(1) {
    // TODO: we need to ensure that 0 < _w < 1!
  }
  
  
  UAExp::~UAExp(void) {
  }
  
  
  double UAExp::getCurrentUtility(void) const {
    return currentValue;
  }
  
  
  double UAExp::predictUtility(double u) const {
    return calcValue(u);
  }
  
  
  void UAExp::addHook(double u) {
    currentValue = calcValue(u);
  }
  
  
  double UAExp::calcValue(double u) const {
    return ( (1 - weight) * currentValue ) + weight * u;
  }
  
  double UAExp::getWeight() const {
    return this->weight;
  }
  
  void UAExp::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer,(xmlChar*) "uaexp");
    xmlTextWriterWriteElement(writer, (xmlChar*)"weight", STRTOXML(XmlUtils::convertToXML<double>(this->weight)));
    xmlTextWriterEndElement(writer);
  }

  UtilityAggregator* UAExp::clone() const {
    UtilityAggregator* ua = new UAExp(this->weight);
    return ua;
  }


  const std::string& UAExp::getClassElement() {
    return ELEM_NAME;
  }


  int UAExp::writeData(xmlTextWriterPtr writer) {
    UtilityAggregator::writeData(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"weight", STRTOXML(XmlUtils::convertToXML<double>(this->weight)));
    return 0;
  }
  
} // NS tmssim
