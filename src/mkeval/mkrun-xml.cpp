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
 * $Id: mkrun-xml.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file runts.cpp
 * @brief Run a single (m,k) task set from XML
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 * @todo Combine this somehow with mkrun, does nearly the same
 */

#include <utils/logger.h>
#include <utils/tlogger.h>

#include <mkeval/mkeval.h>
#include <mkeval/mkgenerator.h>

#include <taskmodels/mktask.h>

#include <xmlio/tasksetreader.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;
using namespace tmssim;

#include <boost/program_options.hpp>
namespace po = boost::program_options; 


const int DEFAULT_SIMULATION_STEPS = 100;

/**
 * @brief Initialise the program_options object
 */
void initialiseProgramOptions();

/**
 * @brief Initialise the program variables from the program_options
 */
bool initialise();

/**
 * Print the usage message that describes all parameters
 */
void printUsage(char* prog);

bool checkGCfg(void);
void performSimulation();
void printLegend();


/// @name Program options
/// @{
po::options_description desc;
po::variables_map vm;
/// @brief input file (-i)
string theInputFile = "";
/// @brief path to econf file
string poEconfFile;
/// @brief number of simulation steps
TmsTime theSimulationSteps;
/// @brief configuration of logger
vector<string> poLog;
/// @brief allocators (= scheduling scenarios)
vector<string> poAllocators;
/// @}


/// @brief Input data
vector<MkTask*> mkTasks;

/// @brief Econf file
KvFile* econf = NULL;
SchedulerConfiguration scc;

vector<const MkEvalAllocatorPair*> theAllocators;
unsigned nEvals;


int main(int argc, char* argv[]) {
  logger::setClass(0);

  initialiseProgramOptions();

  try {
    store(parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
      printUsage(argv[0]);
      return 1;
    }
    notify(vm);
  }
  catch (po::error& e) {
    tError() << "ERROR: " << e.what();
    tError() << "Use '--help' for further information.";
    return -1;
  }
  
  if (!initialise()) {
    tError() << "Initialisation failed, see error messages above. Exiting.";
    return -1;
  }

  printLegend();
  performSimulation();

}


void initialiseProgramOptions() {  
  desc.add_options()
    ("help,h", "produce help message")
    ("infile,i", po::value<string>(&theInputFile)->required(), "xml input file")
    ("econf,e", po::value<string>(&poEconfFile), "KvFile with settings for taskset execution")
    (",n", po::value<TmsTime>(&theSimulationSteps)->default_value(DEFAULT_SIMULATION_STEPS), "Simulation steps")
    (",a", po::value<vector<string>>(&poAllocators)->required(), "Scheduler/Task allocators, for valid options see below")
    (",l", po::value<vector<string>>(&poLog), "Activate execution logs, for valid options see below")
    ;
}


#define INITIALISE_FAIL success = false;	\
  goto initialise_end;

bool initialise() {
  cout << "Initialising program..." << endl;
  bool success = true;

  TasksetReader* reader = TasksetReader::getInstance();
  Taskset tmpTasks;
  if (reader->read(theInputFile, tmpTasks)) {
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
    }
    catch (KvFileException& e) {
    tError() << "Error when reading econf file " << poEconfFile
	     << ": " << e.error;
    INITIALISE_FAIL;
    }
    scc = SchedulerConfiguration(econf);
  }
  
  // Logger
  for (string logClass: poLog) {
    logger::activateClass(logClass);
  }
  cout << "Log class: " << hex << logger::getCurrentClass() << dec << endl;

  // allocators
  for (const string& alloc: poAllocators) {
    const MkEvalAllocatorPair* ap = MkAllocators::instance().getAllocatorPair(alloc);
    if (ap == NULL) {
      tError() << "Unknown allocator: " << alloc;
      INITIALISE_FAIL;
    }
    vector<const MkEvalAllocatorPair*>::iterator it = theAllocators.begin();
    while (it != theAllocators.end() && (*it)->id < alloc) {
      ++it;
    }
    theAllocators.insert(it, ap);
  }
  // now we know how many evals must be performed:
  nEvals = theAllocators.size();

 initialise_end:
  return success;
}


void printUsage(char* prog) {
  //cout << __FUNCTION__ << " not implemented yet" << endl;
  
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


void performSimulation() {
  MkTaskset mkts;
  mkts.tasks = mkTasks;
  
  cout << "Original tasks:" << endl;
  for (vector<MkTask*>::iterator it = mkts.tasks.begin();
       it != mkts.tasks.end(); ++it) {
    cout << *(*it) << endl;
  }

  MkEval* eval = new MkEval(&mkts, nEvals, theAllocators, scc, theSimulationSteps);
  eval->run();
  const MKSimulationResults* results = eval->getResults();
  for (unsigned int i = 0; i < nEvals; ++i) {
    cout << results[i];
  }

  cout << "Simulated cycles: " << results->simulatedTime << endl;
  
  cout << "Simulated tasks:" << endl;
  const vector<MkTask*>* tasks = eval->getSimTasks(0);
  for (MkTask* t: *tasks) {
    cout << *t << endl;
  }

}


void printLegend() {
  cout << "Job: J[taskref],[jobnum] ([remET]/[ET]-[ct] P [priority] S [st])\n";
}
