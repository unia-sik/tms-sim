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
 * $Id: tsprint.cpp 1432 2016-07-15 10:43:04Z klugeflo $
 * @file tsprint.cpp
 * @brief Print (m,k)-taskset from XML in readable format
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <utils/bitstrings.h>
#include <utils/logger.h>
#include <utils/tlogger.h>

#include <mkeval/mkeval.h>
#include <mkeval/gstsimulation.h>

#include <schedulers/schedulers.h>

#include <taskmodels/mktask.h>
#include <taskmodels/mkptask.h>
#include <taskmodels/dbptask.h>

#include <xmlio/tasksetreader.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

#include <boost/program_options.hpp>


using namespace std;
using namespace tmssim;
namespace po = boost::program_options; 


/**
 * @brief Initialise the program_options object
 */
void initialiseProgramOptions();

/**
 * Print the usage message that describes all parameters
 */
void printUsage(char* prog);

bool initialise();
void performSimulation();
void printLegend();

/// @name Program options
/// @{
po::options_description desc;
po::variables_map vm;
/// @brief input file name
string poFileName;
/// @brief allocator (= scheduling scenarios)
string poAllocator;
/// @brief logging options
vector<string> poLog;
const MkEvalAllocatorPair* theAllocator;
/// @}


list<MkTask*> mkTasks;
//map<string, size_t> allocatorIdMap;
//unsigned nEvals;


int main(int argc, char* argv[]) { 
  // INIT
  logger::setClass(0);
  
  initialiseProgramOptions();
  
  try {
    po::store(parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
      printUsage(argv[0]);
      return 1;
    }
    po::notify(vm);
  }
  catch (po::error& e) {
    cerr << "ERROR: " << e.what() << endl;
    cerr << "Use '--help' for further information" << endl;
    return -1;
  }
  
  if (!initialise()) {
    tError() << "Initialisation failed, see error messages above. Exiting.";
    return -1;
  }

  cout << "Id T C m k u_i m/k" << endl;
  //list<MkTask*> concreteTasks;
  double u = 0;
  double umk = 0;
  for (MkTask* t: mkTasks) {
    //cout << *t << endl;
    double u_i = ((float)t->getExecutionTime()/t->getPeriod());
    double m_k = ((float)t->getM()/t->getK());
    cout << t->getIdString() << " " << t->getPeriod() << " "
	 << t->getExecutionTime() << " " << t->getM() << " "
	 << t->getK() << " " << u_i << " "
	 << m_k << endl;
    u += u_i;
    umk += u_i * m_k;
  }

  cout << "U: " << u << " Umk: " << umk << endl;
  /*
  MkExtTaskset* mkets = new MkExtTaskset(concreteTasks, theAllocator->schedAlloc());

  cout << "Hyperperiod: " << mkets->getHyperPeriod() << endl;
  cout << "FeasibilityMultiplicator: " << mkets->getMkFeasibilityMultiplicator() << endl;
  cout << "FeasibilityInterval: " << mkets->getMkFeasibilityInterval() << endl;
  

  delete mkets;
  */
}


/*
  Good overview of how to use boost::program_options:
  http://www.radmangames.com/programming/how-to-use-boost-program_options
*/
void initialiseProgramOptions() {  
  desc.add_options()
    ("help,h", "produce help message")
    (",i", po::value<string>(&poFileName)->required(), "XML (m,k)-taskset file")
    ;
}


void printUsage(char* prog) {
  cout << "Usage: " << prog << " [arguments] \nArguments:" << endl;
  /*
    TODO: nicer output, e.g. with output class from 
    http://www.radmangames.com/programming/how-to-use-boost-program_options
  */
  cout << desc << "\n";
}



#define INITIALISE_FAIL success = false;		\
  goto init_end;

bool initialise() {
  bool success = true;

  TasksetReader* reader = TasksetReader::getInstance();
  Taskset tmpTasks;
  if (reader->read(poFileName, tmpTasks)) {
    for (Task* t: tmpTasks) {
      MkTask* mkt = dynamic_cast<MkTask*>(t);
      if (mkt != NULL) {
	mkTasks.push_back(mkt);
      }
      else {
	tError() << "Unable to convert task " << *t;
	INITIALISE_FAIL;
      }
    }
  }
  
 init_end:
  return success;
}

