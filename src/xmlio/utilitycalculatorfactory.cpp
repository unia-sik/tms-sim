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
 * $Id: utilitycalculatorfactory.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file utilitycalculatorfactory.cpp
 * @brief Creation of utility calculators from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/utilitycalculatorfactory.h>

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <exception>
#include <stdexcept>

#include <utils/tlogger.h>
#include <xmlio/ucfirmrtbuilder.h>

using namespace std;

namespace tmssim {

UtilityCalculatorFactory::UtilityCalculatorFactory() {
	mBuilder = map<string, IElementBuilder<UtilityCalculator>*>();
	map<string, IElementBuilder<UtilityCalculator>*>::iterator it = mBuilder.begin();
	mBuilder.insert(it, pair<string, IElementBuilder<UtilityCalculator>*>("ucfirmrt", new UCFirmRTBuilder()));
}

bool UtilityCalculatorFactory::accept(__attribute__((unused)) xmlDocPtr doc, xmlNodePtr cur) {
	const xmlChar* strPtr = cur->name;
	string s((char*) strPtr);
	return mBuilder.find(s) != mBuilder.end();
}

UtilityCalculator* UtilityCalculatorFactory::getElement(xmlDocPtr doc, xmlNodePtr cur) {
  //tDebug() << "getElement: " << cur->name;
	try {
		return mBuilder.at((const char*) cur->name)->build(doc, cur);
	} catch (out_of_range& ex) {
		//if (mBuilder.find() == mBuilder.end()) {
		//int keyCount = 1;//mBuilder.count((const char*) cur->name);
		//if (keyCount == 0) {
	  tError() << "Key is NULL";
		return NULL;

		stringstream ss;
		ss << "Key in map not found: " << cur->name << endl;
		string error = ss.str();
		throw invalid_argument(error);
		//}
	}
}

UtilityCalculatorFactory::~UtilityCalculatorFactory() {
  for (map<string, IElementBuilder<UtilityCalculator>*>::iterator it = mBuilder.begin(); it != mBuilder.end(); ++it) {
    delete it->second;
  }
}

} // NS tmssim
