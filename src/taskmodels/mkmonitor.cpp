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
 * $Id: mkmonitor.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file mkmonitor.cpp
 * @brief Monitoring of (m,k)-firm condition
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <taskmodels/mkmonitor.h>

#include <cassert>
#include <sstream>

using namespace std;

namespace tmssim {

  MkMonitor::MkMonitor(unsigned int _m, unsigned int _k, CompressedMkState _is /*, unsigned int _init*/)
    : m(_m), k(_k), pos(0), recorded(0), violations(0)
      // Initialise recorded with 1 to avoid additions during comparisons
  {
    assert(k < 64);
    //unsigned int _init = 1;
    vals = new unsigned int[k];
    sum = 0;
    for (unsigned int i = 0; i < k; ++i) {
      unsigned iv = (_is >> i) & 1;
      vals[i] = iv;
      sum += iv;
    }
  }


  MkMonitor::~MkMonitor() {
    delete[] vals;
  }


  void MkMonitor::push(unsigned int _val) {
    unsigned int tmp = vals[pos];
    vals[pos] = _val;
    sum -= tmp;
    sum += _val;
    ++pos;
    if (pos >= k)
      pos = 0;
    ++recorded;
    if (recorded >= k) { // recorded == k <=> the newest value was the k-th one
      if (sum < m)
	++violations;
    }
    assert(pos < k);
  }


  unsigned int MkMonitor::getCurrentSum() const {
    if (recorded < k) {
      // this is more tricky than it seems - we need to check wether the task
      // may still keep its constraints
      unsigned int missing = k - recorded; // how many values/jobs are missing for full window?
      unsigned int ctr = 0;
      unsigned int psum = sum;
      size_t ppos = pos;
      while (ctr < missing) {
	psum -= vals[ppos];
	++ppos;
	if (ppos >= k)
	  ppos = 0;
	++ctr;
      }
      psum += missing;
      return psum;
    }
    else {
      return sum;
    }
  }


  const unsigned int* MkMonitor::getVals() const {
    return vals;
  }


  size_t MkMonitor::getPos() const {
    return pos;
  }


  unsigned int MkMonitor::getViolations() const {
    return violations;
  }


  string MkMonitor::printState() const {
    ostringstream oss;
    for (size_t i = 0; i < k; ++i) {
      size_t p = (pos + i) % k;
      oss << vals[p];
    }
    return oss.str();
  }


  CompressedMkState MkMonitor::getState() const {
    if (k > CMKS_MAX_SIZE) {
      ostringstream oss;
      oss << "State for k=" << k << " cannot be fit into CompressedMkState";
      throw MkMonitor::MkMonitorException(oss.str());
    }
    CompressedMkState cstate = 0;
    for (size_t i = 0; i < k; ++i) {
      size_t p = (pos + i) % k;
      //cstate |= vals[p] << i;
      cstate <<= 1;
      cstate |= vals[p] & 0x1;
    }
    return cstate;
  }

  CompressedMkState MkMonitor::getReducedState() const {
    if (k > CMKS_MAX_SIZE) {
      ostringstream oss;
      oss << "State for k=" << k << " cannot be fit into CompressedMkState";
      throw MkMonitor::MkMonitorException(oss.str());
    }
    CompressedMkState cstate = 0;
    for (size_t i = 1; i < k; ++i) {
      size_t p = (pos + i) % k;
      //cstate |= vals[p] << i;
      cstate <<= 1;
      cstate |= vals[p] & 0x1;
    }
    return cstate;
  }

  
  bool MkMonitor::isStateValid() const {
    /*
    if (recorded < k)
      return true;
    else
      return sum >= m;
    */
    // have to use internal function!
    return getCurrentSum() >= m;
  }

} // NS tmssim
