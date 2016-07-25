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
 * $Id: uawmean.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file uawmean.cpp
 * @brief Implementation of aggregator over sliding window with average calculation
 * @authors Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

#include <utility/uawmean.h>

#include <xmlio/xmlutils.h>

namespace tmssim {

  static const string ELEM_NAME = "uawmean";

  UAWMean::UAWMean(size_t _size)
    : UAWindow(_size), currentSum(0)
  {
  }
  
  
  UAWMean::~UAWMean(void) {
  }
  
  
  double UAWMean::getCurrentUtility(void) const {
    return currentSum / getSize();
  }
  
  
  double UAWMean::predictUtility(double u) const {
    double s = currentSum;
    // need to subtract oldest entry
    s -= oldest();
    s += u;
    return s / getSize();
  }
  
  
  void UAWMean::addHook(double u) {
    currentSum -= oldest();
    UAWindow::addHook(u);
    currentSum += u;
  }


  UtilityAggregator* UAWMean::clone() const {
    UtilityAggregator* ua = new UAWMean(this->getSize());
    return ua;
  }

  
  void UAWMean::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer,(xmlChar*) "uawmean");
    xmlTextWriterWriteElement(writer, (xmlChar*)"size", STRTOXML(XmlUtils::convertToXML<size_t>(this->getSize())));
    xmlTextWriterEndElement(writer);
  }


  const std::string& UAWMean::getClassElement() {
    return ELEM_NAME;
  }


  int UAWMean::writeData(xmlTextWriterPtr writer) {
    UAWindow::writeData(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"size", STRTOXML(XmlUtils::convertToXML<size_t>(this->getSize())));
    return 0;
  }

} // NS tmssim
