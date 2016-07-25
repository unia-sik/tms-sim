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
 * $Id: taskfactory.cpp 1422 2016-06-22 08:59:13Z klugeflo $
 * @file taskfactory.cpp
 * @brief Creation of tasks from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/taskfactory.h>

#include <iostream>
#include <sstream>
#include <exception>
#include <stdexcept>

#include <utils/tlogger.h>
#include <xmlio/sporadictaskbuilder.h>
#include <xmlio/periodictaskbuilder.h>
#include <xmlio/mktaskbuilder.h>
#include <xmlio/sptaskbuilder.h>
#include <xmlio/dbptaskbuilder.h>
#include <xmlio/pproducertaskbuilder.h>
#include <xmlio/pconsumertaskbuilder.h>

using namespace std;

namespace tmssim {

  TaskFactory::TaskFactory() {
    mBuilder = map<string, IElementBuilder<Task>*>();
    map<string, IElementBuilder<Task>*>::iterator it = mBuilder.begin();
    mBuilder.insert(it, pair<string, IElementBuilder<Task>*>("periodictask", new PeriodicTaskBuilder()));
    mBuilder.insert(it, pair<string, IElementBuilder<Task>*>("sporadictask", new SporadicTaskBuilder()));
    mBuilder.insert(it, pair<string, IElementBuilder<Task>*>("mktask", new MkTaskBuilder()));
    //mBuilder.insert(it, pair<string, IElementBuilder<Task>*>("dbptask", new DbpTaskBuilder()));
    mBuilder.insert(it, pair<string, IElementBuilder<Task>*>("sptask", new SPTaskBuilder()));
    mBuilder.insert(it, pair<string, IElementBuilder<Task>*>("pproducertask", new PProducerTaskBuilder()));
    mBuilder.insert(it, pair<string, IElementBuilder<Task>*>("pconsumertask", new PConsumerTaskBuilder()));
    
  }
  
  TaskFactory::~TaskFactory() {
    for (map<string, IElementBuilder<Task>*>::iterator it = mBuilder.begin();
	 it != mBuilder.end(); ++it) {
      
      delete it->second;
      
    }
  }
  
  
  bool TaskFactory::accept(__attribute__((unused)) xmlDocPtr doc, xmlNodePtr cur) {
    const xmlChar* strPtr = cur->name;
    string s((char*) strPtr);
    return mBuilder.find(s) != mBuilder.end();
  }
  
  Task* TaskFactory::getElement(xmlDocPtr doc, xmlNodePtr cur) {
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

} // NS tmssim
