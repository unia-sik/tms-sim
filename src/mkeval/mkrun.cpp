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
 * $Id: mkrun.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file mkrun.cpp
 * @brief Run a single generated (m,k) task set
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 * @todo Combine this somehow with mkrun-xml, does nearly the same
 */

#include <utils/logger.h>
#include <utils/tlogger.h>
#include <utils/kvfile.h>

#include <mkeval/mkallocators.h>
#include <mkeval/mkeval.h>
#include <mkeval/mkgenerator.h>

#include <taskmodels/mktask.h>

#include <xmlio/tasksetwriter.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;
using namespace tmssim;

#include <boost/program_options.hpp>
namespace po = boost::program_options; 


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
/// @brief path to config file
string poConfigFile;
/// @brief path to econf file
string poEconfFile;
/// @brief seed parameter
unsigned poSeed;
/// @brief taskset size
unsigned theTasksetSize;
/// @brief how many taskset to generate
unsigned theNTasksets;
/// @brief number of simulation steps
TmsTime theSimulationSteps;
/// @brief number of simulation threads parameter (-m)
unsigned theNThreads;
/// @brief target utilisation
double theUtilisation;
/// @brief maximum allowed deviation from generation utilisation
double theUtilisationDeviation;
/// @brief log prefix
string theLogPrefix = "";
/// @brief configuration of logger
vector<string> poLog;
/// @brief allocators (= scheduling scenarios)
vector<string> poAllocators;
/// @brief output file (optional)
string theOutput = "";
/// @}

/// @name Actual parameters
/// @{

const int DEFAULT_SIMULATION_STEPS = 100;
const int DEFAULT_TASKSET_SIZE = 5;
const int DEFAULT_N_TASKSETS = 100;
const double DEFAULT_UTILISATION = 1.0;
const double DEFAULT_UTILISATION_DEVIATION = 0.1;

/// @brief Seed used for taskset generation
unsigned int theSeed;

/// @brief internal representation of generator configuration file
KvFile* gcfg = NULL;

/// @brief Econf file
KvFile* econf = NULL;
SchedulerConfiguration scc;

vector<const MkEvalAllocatorPair*> theAllocators;
unsigned nEvals;

/// @}


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

  delete gcfg;
  if (econf)
    delete econf;

}


void initialiseProgramOptions() {  
  desc.add_options()
    ("help,h", "produce help message")
    ("config,c", po::value<string>(&poConfigFile)->required(), "KvFile with settings for the task set generator (mandatory)")
    ("econf,e", po::value<string>(&poEconfFile), "KvFile with settings for taskset execution")
    ("seed,s", po::value<unsigned>(&poSeed), "Seed [default=time(NULL)]")
    (",t", po::value<unsigned>(&theTasksetSize)->default_value(DEFAULT_TASKSET_SIZE), "Taskset size")
    (",n", po::value<TmsTime>(&theSimulationSteps)->default_value(DEFAULT_SIMULATION_STEPS), "Simulation steps")
    (",u", po::value<double>(&theUtilisation)->default_value(DEFAULT_UTILISATION), "Target utilisation")
    (",d", po::value<double>(&theUtilisationDeviation)->default_value(DEFAULT_UTILISATION_DEVIATION), "Deviation of actual utilisation for target utilisation")
    (",a", po::value<vector<string>>(&poAllocators)->required(), "Scheduler/Task allocators, for valid options see below")
    ("outfile,o", po::value<string>(&theOutput), "xml output file, if the file exists it will be truncated (optional)")
    (",l", po::value<vector<string>>(&poLog), "Activate execution logs, for valid options see below")
    ;
}


#define INITIALISE_FAIL success = false;	\
  goto initialise_end;

bool initialise() {
  cout << "Initialising program..." << endl;
  bool success = true;

  // config file:
  try {
    gcfg = new KvFile(poConfigFile);
    if (!checkGCfg()) {
      tError() << "Invalid configuration file!";
      INITIALISE_FAIL;
    }
    else {
      tInfo() << "Configuration file read successfully!";
    }
  }
  catch (KvFileException& e) {
    tError() << "Error when reading config file " << poConfigFile
	     << ": " << e.error;
    INITIALISE_FAIL;
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
  
  // seed
  if (vm.count("seed")) {
    theSeed = poSeed;
  }
  else {
    theSeed = time(NULL);
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


bool checkGCfg(void) {
  return gcfg->containsKey("minPeriod")
    && gcfg->containsKey("maxPeriod")
    && gcfg->containsKey("minK")
    && gcfg->containsKey("maxK")
    && gcfg->containsKey("maxWC");
}


void performSimulation() {
  MkGenerator generator(theSeed, theTasksetSize,
			gcfg->getUInt32("minPeriod"), gcfg->getUInt32("maxPeriod"),
			gcfg->getUInt32("minK"), gcfg->getUInt32("maxK"),
			theUtilisation, theUtilisationDeviation, gcfg->getUInt32("maxWC"));

  MkTaskset* mkts = generator.nextTaskset(theSeed);
  if (vm.count("outfile")) {
      TasksetWriter* tsw = TasksetWriter::getInstance();
      vector<Task*> tasks;
      for (MkTask* t: mkts->tasks) {
	tasks.push_back(t);
      }
      bool result = tsw->write(theOutput, tasks);
      //TasksetWriter::clear();
      if (!result) {
	tError() << "Writing task set failed!";
      }
  }
  MkEval* eval = new MkEval(mkts, nEvals, theAllocators, scc, theSimulationSteps);
  eval->run();
  const MKSimulationResults* results = eval->getResults();
  for (unsigned int i = 0; i < nEvals; ++i) {
    cout << results[i];
  }

  for (vector<MkTask*>::iterator it = mkts->tasks.begin();
       it != mkts->tasks.end(); ++it) {
    cout << *(*it) << endl;
  }

}


void printLegend() {
  cout << "Job: J[taskref],[jobnum] ([remET]/[ET]-[ct] P [priority] S [st])\n";
}
