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
 * $Id: ucfirm.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file ucfirm.cpp
 * @brief Implementation of firm real-time utility calculator
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

#include <utility/ucfirm.h>

namespace tmssim {

  static const string ELEM_NAME = "ucfirmrt";

  UCFirmRT::UCFirmRT() {
  }
  
  
  UCFirmRT::~UCFirmRT(void) {
  }
  
  
  double UCFirmRT::calcUtility(const Job *job, TmsTime complTime) const {
    if (complTime <= job->getAbsDeadline()) {
      return 1;
    }
    else {
      return 0;
    }
  }
  
  UtilityCalculator* UCFirmRT::clone() const {
    UtilityCalculator* uc = new UCFirmRT();
    return uc;
  }
  
  void UCFirmRT::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer,(xmlChar*) "ucfirmrt");
    xmlTextWriterEndElement(writer);
  }


  const std::string& UCFirmRT::getClassElement() {
    return ELEM_NAME;
  }


  int UCFirmRT::writeData(xmlTextWriterPtr writer) {
    UtilityCalculator::writeData(writer);
    return 0;
  }

} // NS tmssim
