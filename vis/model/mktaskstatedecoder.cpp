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
 * $Id: mktaskstatedecoder.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file mktaskstatedecoder.cpp
 * @brief Decoding of (m,k)-tasks
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include "mktaskstatedecoder.h"

#include <boost/regex.hpp>

#include <clocale>

#include <iostream>

using namespace std;
using namespace boost;

// Task Description
// (7/2/7) [0;1] (3,4) s=0
// (6/1/6) [0;1] (2,5) spin=1 mkstate = 11111
const regex RE_MKTASK_DATA("\\(([0-9]+)/([0-9]+)/([0-9]+);[0-9]+\\) \\[[0-9\\.]*;[0-9\\.]*\\] \\(([0-9]+),([0-9]+)\\) spin=([0-9]+) mkstate = [01]*");

// Task State
// [1011011010] 6/10 D=7 | 3->2.5
// [11111] 5/5 D=3 | 1.66667->1.33333
// \s*\[([01]+)\] ([0-9]+)\/([0-9]+) D=([0-9]+) \| ([0-9\.]+)->([0-9\.]+)
const regex RE_MKTASK_STATE("\\s*\\[([01]+)\\] ([0-9]+)/([0-9]+) D=([0-9]+) \\| ([0-9\\.]+)->([0-9\\.]+)\\s*");

MkTaskStateDecoder::MkTaskStateDecoder(std::string __taskDescription)
  : TaskStateDecoder(__taskDescription) {
  smatch sm;
  if (regex_match(taskData, sm, RE_MKTASK_DATA)) {
    period = stoi(sm[1]);
    executionTime = stoi(sm[2]);
    deadline = stoi(sm[3]);
    m = stoi(sm[4]);
    k = stoi(sm[5]);
    spin = stoi(sm[6]);
    /*cout << "Decoded (m,k)-task, T=" << period << " C=" << executionTime
	 << " D=" << deadline << " (m,k)=(" << m << "," << k << ") s="
	 << spin << endl;*/
  }
  else {
    cerr << "Could not decode (m,k)-task data from \"" << taskData << "\"" << endl;
  }
}


MkTaskStateDecoder::MkTaskStateDecoder(const MkTaskStateDecoder& rhs)
  : TaskStateDecoder(rhs), period(rhs.period), executionTime(rhs.executionTime),
    deadline(rhs.deadline), m(rhs.m), k(rhs.k), spin(rhs.spin) {
}



MkTaskStateDecoder::~MkTaskStateDecoder() {
}


/*
    oss << "{ [" << monitor.printState() << "] " << monitor.getCurrentSum() << "/" << k << " D=" << getDistance() << " | " << getUA()->getCurrentUtility() << "->" << getUA()->predictUtility(0) << "}";
 */
void MkTaskStateDecoder::setState(const std::string& _state) {
  smatch sm;
  if (regex_match(_state, sm, RE_MKTASK_STATE)) {
    //cout << "RE-M: " << sm << endl;
    mkstream = sm[1];
    streamSum = stoi(sm[2]);
    distance = stoi(sm[4]);
    // switch to C locale for decoding of floating point numbers
    // as output produced by tms-sim is based on C locale
    string oldLocale = setlocale(LC_ALL, NULL);
    setlocale(LC_ALL, "C");
    uCurrent = stod(sm[5]);
    uPredict = stod(sm[6]);
    // restore locale
    setlocale(LC_ALL, oldLocale.c_str());
    /*
    cout << "Decoded mkjob, uCurrent=" << uCurrent << "(" << sm[5] << ")"
	 << " uPredict=" << uPredict << "(" << sm[6] << ")"
	 << endl;
    */
  }
  else {
    cerr << "Could not decode (m,k)-task state from \"" << _state << "\"" << endl;
    cerr << "against: \"" << RE_MKTASK_STATE << "\"" << endl;
  }
}
