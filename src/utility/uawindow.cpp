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
 * $Id: uawindow.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file uawindow.cpp
 * @brief Implementation of aggregator over a sliding window
 * @authors Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

#include <utility/uawindow.h>

#include <utils/tlogger.h>

#include <cassert>
using namespace std;
namespace tmssim {

  UAWindow::UAWindow(size_t _size, double initValue)
    : UtilityAggregator(), size(_size), current(_size-1)
  {
    values = new double[size];
    for (unsigned int i = 0; i < size; ++i) {
      values[i] = initValue;
    }
  }


  UAWindow::UAWindow(const UAWindow& rhs)
    : UtilityAggregator(), size(rhs.size), current(size-1)
  {
    values = new double[size];
    for (unsigned int i = 0; i < size; ++i) {
      values[i] = rhs.values[i];
    }
  }
  
  
  UAWindow::~UAWindow(void) {
    delete[] values;
  }
  
  
  void UAWindow::addHook(double u) {
    if (current == 0) current += size;
    --current;
    tDebug() << "current @ addHook: " << current;
    assert(current < size);// && current >= 0);
    values[current] = u;
  }
  
  
  double UAWindow::operator [](size_t offset) const {
    return values[(current+offset)%size];
  }
  
  
  size_t UAWindow::getSize(void) const {
    return size;
  }
  
  
  double UAWindow::oldest(void) const {
    return (*this)[size-1];
  }


  int UAWindow::writeData(xmlTextWriterPtr writer) {
    return UtilityAggregator::writeData(writer);
  }

} // NS tmssim
