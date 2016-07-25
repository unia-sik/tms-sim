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
 * $Id: excrun.cpp 1432 2016-07-15 10:43:04Z klugeflo $
 * @file excrun.cpp
 * @brief Run task set with exact schedulability test
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <utils/bitstrings.h>
#include <utils/logger.h>

// #ifdef TLOGLEVEL
// #undef TLOGLEVEL
// #endif
// #define TLOGLEVEL TLL_INFO

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
//using namespace boost::program_options; 
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
/// @brief path to econf file
string poEconfFile;
/// @brief allocator (= scheduling scenarios)
string poAllocator;
/// @brief logging options
vector<string> poLog;
const MkEvalAllocatorPair* theAllocator;
/// @}

/// @brief Econf file
KvFile* econf = NULL;
SchedulerConfiguration scc;

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

  cout << "Using allocator: " << theAllocator->id << endl;
  
  list<MkTask*> concreteTasks;
  for (MkTask* t: mkTasks) {
    //cout << "\t" << *t << endl;
    MkTask* ta = theAllocator->taskAlloc(t);
    cout << *ta << endl;;
    concreteTasks.push_back(ta);
  }

  //GstSimulation* mkets = new GstSimulation(concreteTasks, theAllocator->schedAlloc(scc), theAllocator->id);
  MkSimulation* mksim = theAllocator->simAlloc(concreteTasks, theAllocator->schedAlloc(scc), theAllocator->id);

  cout << mksim->getInfoMessage();
  /*
  cout << "Hyperperiod: " << mkets->getHyperPeriod() << endl;
  cout << "FeasibilityMultiplicator: " << mkets->getMkFeasibilityMultiplicator() << endl;
  cout << "FeasibilityInterval: " << mkets->getMkFeasibilityInterval() << endl;
  */
  bool rv = mksim->simulate();
  /*
  cout << "Simulation returned " << rv << " after " << mkets->getHpCount()-1
       << " hyperperiods in cycle " << mkets->getSimulation()->getTime() << "." << endl;
  */
  cout << mksim->getSimulationMessage();
  
  GstSimulation* mkets = dynamic_cast<GstSimulation*>(mksim);
  if (mkets != NULL) {
    if (rv) {
      cout << "Simulation successfull";
      if (mkets->stateRecurred())
	cout << " due to recurring state";
      cout << endl;
      cout << "Last task state at hyperperiod:";
      const CompressedMkState* states = mkets->getLastMkStates();
      size_t i = 0;
      for (MkTask* task: mkTasks) {
	cout << " ";
	printBitString(cout, states[i], task->getK());
	++i;
    }
      cout << endl;
    }
    else {
      cout << "Scheduling failed." << endl;
      cout << "Last task state at hyperperiod:";
      const CompressedMkState* states = mkets->getLastMkStates();
      size_t i = 0;
      for (MkTask* task: mkTasks) {
	cout << " ";
	printBitString(cout, states[i], task->getK());
	++i;
      }
      cout << endl;
    }
  }
  /*
  printLegend();
  performSimulation();
  */

  delete mksim;
  if (econf)
    delete econf;

  return 0;
}


/*
  Good overview of how to use boost::program_options:
  http://www.radmangames.com/programming/how-to-use-boost-program_options
*/
void initialiseProgramOptions() {  
  desc.add_options()
    ("help,h", "produce help message")
    (",i", po::value<string>(&poFileName)->required(), "XML (m,k)-taskset file")
    ("econf,e", po::value<string>(&poEconfFile), "KvFile with settings for taskset execution")
    (",a", po::value<string>(&poAllocator)->required(), "Scheduler/Task allocator, for valid options see below")
    (",l", po::value<vector<string>>(&poLog), "Activate execution logs, for valid options see below")
    ;
}


void printUsage(char* prog) {
  cout << "Usage: " << prog << " [arguments] \nArguments:" << endl;
  /*
    TODO: nicer output, e.g. with output class from 
    http://www.radmangames.com/programming/how-to-use-boost-program_options
  */
  cout << desc << "\n";
  cout << "Valid parameters for the -a option are:" << endl;
  list<string> al = MkAllocators::instance().listAllocators();
  for (const string& alloc: al) {
    cout << "\t" << alloc << endl;
  }
  cout << "Valid parameters for the -l option are:";
  for (size_t i = 0; i < LOG_N_PREFIX; ++i) {
    cout << " " << tmssim::detail::LOG_PREFIX[i];
  }
  cout << endl;
  cout << "The -a, -l and -u options may be specified multiple times." << endl;
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

  // econf
  if (vm.count("econf")) {
    try {
      econf = new KvFile(poEconfFile);
      tInfo() << "EConf file read successfully!";
      cout << "EConf file read successfully!" << endl;
    }
    catch (KvFileException& e) {
    tError() << "Error when reading econf file " << poEconfFile
	     << ": " << e.error;
    INITIALISE_FAIL;
    }
    scc = SchedulerConfiguration(econf);
    cout << "Scheduler config: "
	 << "SCC EC: " << scc.execCancellations
	 << " DLMC: " << scc.dlMissCancellations
	 << endl;

  }
  
  // allocators
  //for (const string& alloc: poAllocators) {
  theAllocator = MkAllocators::instance().getAllocatorPair(poAllocator);
  /*  
    if (ap == NULL) {
      tError() << "Unknown allocator: " << alloc;
      INITIALISE_FAIL;
    }
    vector<const MkEvalAllocatorPair*>::iterator it = theAllocators.begin();
    while (it != theAllocators.end() && (*it)->id < alloc) {
      ++it;
    }
    theAllocators.insert(it, ap);
    }*/
  // now we know how many evals must be performed:
  /*
  nEvals = theAllocators.size();
  for (size_t i = 0; i < nEvals; ++i) {
    allocatorIdMap[theAllocators[i]->id] = i;
  }
  */
  
  // Logger
  for (string logClass: poLog) {
    logger::activateClass(logClass);
  }
  //cout << "Log class: " << hex << logger::getCurrentClass() << dec << endl;

  
 init_end:
  return success;
}

