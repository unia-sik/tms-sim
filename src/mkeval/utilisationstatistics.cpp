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
 * $Id: utilisationstatistics.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file utilisationstatistics.cpp
 * @brief Statistics on task sets' processor utilisation
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <mkeval/utilisationstatistics.h>

#include <cassert>
#include <cmath>

#include <utils/tlogger.h>

using namespace std;

namespace tmssim {
  
  UtilisationStatistics::UtilisationStatistics()
    : evaluated(false), mean(-1), sigma(-1), median(-1)
  {
  }
  

  UtilisationStatistics::~UtilisationStatistics() {
  }

  
  void UtilisationStatistics::addUtilisation(double utilisation) {
    assert(utilisation > 0);

    list<double>::iterator it = uList.begin();
    while (it != uList.end()
	   && *it < utilisation ) {
      ++it;
    }
    uList.insert(it, utilisation);
    evaluated = false;
  }

  void UtilisationStatistics::evaluate() {
    int n = uList.size();
    assert(n > 1); // else median computation can fail
    data.clear();
    data.resize(n);
    int offset = 0;
    double sumData = 0;
    for (list<double>::iterator it = uList.begin();
	 it != uList.end(); ++it, ++offset) {
      data[offset] = *it;
      sumData += *it;
    }
    mean = sumData / n;
    double sumVar = 0;
    for (list<double>::iterator it = uList.begin();
	 it != uList.end(); ++it) {
      sumVar += (*it - mean) * (*it - mean);
    }
    var = sumVar / n;
    sigma = sqrt(var);

    if ((n & 1) == 1) { // can take real middle
      median = data[n >> 1];
    }
    else { // n is even, take mean of two middle values
      median = ( data[n >> 1] + data[(n >> 1) - 1] ) / 2;
    }
    evaluated = true;
  }
  

  double UtilisationStatistics::getSize() const {
    return uList.size();
  }

  
  double UtilisationStatistics::getMean() const {
    return evaluated ? mean : -1;
  }
  
  
  double UtilisationStatistics::getSigma() const {
    return evaluated ? sigma : -1;
  }


  double UtilisationStatistics::getMedian() const {
    return evaluated ? median : -1;
  }
  

  double UtilisationStatistics::getMin() const {
    return evaluated ? data.front() : -1;
  }


  double UtilisationStatistics::getMax() const {
    return evaluated ? data.back() : -1;
  }


  double UtilisationStatistics::getMedianIntervalLower(int percent) const {
    if (!evaluated)
      return -1;
    int n = uList.size();
    int intervalLength = round((double)percent/100.0 * (double)n);
    int idx = round((float)n/2.0 - (float)intervalLength/2.0);
    return data[idx];
  }


  double UtilisationStatistics::getMedianIntervalUpper(int percent) const {
    if (!evaluated)
      return -1;
    int n = uList.size();
    int intervalLength = round((double)percent/100.0 * (double)n);
    int idx = round((float)n/2.0 + (float)intervalLength/2.0);
    return data[idx];
  }


} // NS tmssim
