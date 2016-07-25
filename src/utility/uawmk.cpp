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
 * $Id: uawmk.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file uawmk.cpp
 * @brief Utility Aggregator for utility-base (m,k)-firm scheduling
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <utility/uawmk.h>

#include <xmlio/xmlutils.h>

namespace tmssim {

  static const string ELEM_NAME = "uawmk";

  UAWMK::UAWMK(unsigned int _m, unsigned int _k)
    : UAWindow(_k),
      m(_m), k(_k), currentSum(_k)
  {
  }


  UAWMK::UAWMK(const UAWMK& rhs)
    : UAWindow(rhs), m(rhs.m), k(rhs.k), currentSum(rhs.currentSum)
  {
  }


  UAWMK::~UAWMK() {
  }


  double UAWMK::getCurrentUtility(void) const {
    //return currentSum / getSize() * k / m;
    // actually, it's the above, but getSize()==k
    return currentSum / m;
  }


  double UAWMK::predictUtility(double u) const {
    double s = currentSum;
    // need to subtract oldest entry
    s -= oldest();
    s += u;
    return s / m;
  }

    
  void UAWMK::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer,(xmlChar*) "uawmk");
    xmlTextWriterWriteElement(writer, (xmlChar*)"m", STRTOXML(XmlUtils::convertToXML<size_t>(this->m)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"k", STRTOXML(XmlUtils::convertToXML<size_t>(this->k)));
    xmlTextWriterEndElement(writer);
  }

    
  UtilityAggregator* UAWMK::clone() const {
    return new UAWMK(*this);
  }


  void UAWMK::addHook(double u) {
    currentSum -= oldest();
    UAWindow::addHook(u);
    currentSum += u;
  }


  const std::string& UAWMK::getClassElement() {
    return ELEM_NAME;
  }


  int UAWMK::writeData(xmlTextWriterPtr writer) {
    UAWindow::writeData(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"m", STRTOXML(XmlUtils::convertToXML<size_t>(this->m)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"k", STRTOXML(XmlUtils::convertToXML<size_t>(this->k)));
    return 0;
  }


} // NS tmssim
