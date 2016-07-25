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
 * $Id: mkmon.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file mkmontest.cpp
 * @brief Test of mkmonitor states output
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <taskmodels/mkmonitor.h>
#include <utils/bitstrings.h>
using namespace tmssim;

#include <iostream>
using namespace std;

int main(void) {

  MkMonitor m1(2, 5);
  cout << m1.printState() << endl;
  m1.push(0);
  m1.push(1);
  m1.push(0);
  cout << m1.printState() << endl;

  MkMonitor m2(4, 7, 0xf);
  cout << m2.printState() << endl;
  m2.push(0);
  m2.push(1);
  m2.push(0);
  cout << m2.printState() << endl;
  
  return 0;
}
