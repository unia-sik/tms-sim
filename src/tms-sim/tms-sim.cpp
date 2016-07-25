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
 * $Id: tms-sim.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file tms-sim.cpp
 * @brief Timing Models Scheduling SIMulator
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 *
 * @todo check de-/allocation of simulation objects
 */

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <list>
#include <sstream>
#include <vector>
using namespace std;

#include <utils/logger.h>
#include <utils/tlogger.h>
#include <core/scobjects.h>
#include <core/simulation.h>
//#include <core/stat.h>
#include <xmlio/tasksetreader.h>
#include <xmlio/tasksetwriter.h>
#include <xmlio/xmlutils.h>

#include <schedulers/schedulers.h>

#include <taskmodels/taskmodels.h>

#include <utility/aggregators.h>
#include <utility/calculators.h>

using namespace tmssim;

#include <boost/program_options.hpp>
namespace po = boost::program_options; 
//using namespace boost::program_options; 

/// @name Program options
/// @{
po::options_description desc;
po::variables_map vm;
string theInputFile;
string poSchedulers;
TmsTime theSimulationSteps;
/// @brief configuration of logger
vector<string> poLog;
/// @}

const int DEFAULT_SIMULATION_STEPS = 100;

vector<Scheduler*> schedulers;
// TODO: implement new logger
bool verbose = false;
int verboseLevel = 0;


/**
 * @brief Initialise the program_options object
 */
void initialiseProgramOptions();

/**
 * @brief Initialise the program variables from the program_options
 */
bool initialise(vector<Task*>& taskset);

/**
 * Print the usage message that describes all parameters
 */
void printUsage(char* prog);

void printLegend();
void calcStat(const vector<int>& v);
void EvaluateStatistics(vector<SimulationResults>&);

int main(int argc, char *argv[]) {
  //Logger::setLogLevel(Logger::NONE);
  //LOG(0) << "Hello from tms-sim";

  //TLogger::init(TLogger::DEBUG);
  printLegend();
  
  int rv = 0;
  vector<vector<Task*>* > tasksetsForScheduler;
  vector<Simulation*> simulations;
  vector<SimulationResults> statistics;
  
  vector<Task*>* taskset = new vector<Task*>();
  //Logger* logger = NULL;

  initialiseProgramOptions();

  try {
    store(parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
      printUsage(argv[0]);
      goto cleanup;
    }
    notify(vm);
  }
  catch (po::error& e) {
    tError() << "ERROR: " << e.what();
    tError() << "Use '--help' for further information.";
    goto cleanup;
  }
  
  if (!initialise(*taskset)) {
    tError() << "Initialisation failed, see error messages above. Exiting.";
    goto cleanup;
  }

  //logger::setLevel(verboseLevel);
  logger::setClass((1 << verboseLevel) - 1);
  //logger = new Logger(verboseLevel);
  /*
  LOG(0) << "Level 0";
  LOG(1) << "Level 1";
  LOG(2) << "Level 2";
  LOG(3) << "Level 3";
  LOG(4) << "Level 4";
  LOG(5) << "Level 5";
  */
  
  /***************************************************************************
   * Preparing the simulation
   ***************************************************************************/
  
  // For every specified scheduler
  for (size_t j=0; j<schedulers.size(); j++) {
    //tasksetsForScheduler.push_back(new vector<Task*>());
    vector<Task*>* simTaskset = new vector<Task*>;//(taskset->size());
    // Create Copy of the Tasks for each simulation
    for(size_t i=0; i<taskset->size(); i++) {
      //tasksetsForScheduler[j]->push_back(taskset->at(i)->clone());
      simTaskset->push_back(taskset->at(i)->clone());
    }
    //simulations.push_back(new Simulation(tasksetsForScheduler[j],schedulers[j],iterations,verbose));
    simulations.push_back(new Simulation(simTaskset, schedulers[j]));//, logger));
  }
  
  /***************************************************************************
   * Running the simulations
   ***************************************************************************/
  
  
  for (size_t j=0; j<simulations.size(); j++) {
    simulations[j]->run(theSimulationSteps);
    simulations[j]->finalise();
    statistics.push_back(simulations[j]->getResults());
  }
  
  /***************************************************************************
   * Compare the statistics of each simulation
   ***************************************************************************/
  
  EvaluateStatistics(statistics);
  
  /***************************************************************************
   * Cleaning Up
   ***************************************************************************/
  
  // Delete simulations
  for(size_t i=0; i<simulations.size(); i++) {
    delete simulations[i];
  }
  
 cleanup:
  // Delete original taskset
  for(size_t i=0; i<taskset->size(); i++) {
    delete (*taskset)[i];
  }
  delete taskset;
  
  //TasksetReader::getInstance()->clear();
  //TasksetWriter::getInstance()->clear();
  
  return rv;
}


void initialiseProgramOptions() {  
  desc.add_options()
    ("help,h", "produce help message")
    ("input,i", po::value<string>(&theInputFile)->required(), "XML task set")
    (",n", po::value<TmsTime>(&theSimulationSteps)->default_value(DEFAULT_SIMULATION_STEPS), "Simulation steps")
    ("scheduler,s", po::value<string>(&poSchedulers)->required(), "Select at least one scheduler:\n\
- [b]eedf\n\
- [d]vdedf\n\
- [e]df\n\
- [f]pp\n\
- [g]dpa\n\
- gdpa[s]\n\
- [h]cedf\n\
- [m]kuedf\n\
- [p]hcedf")
    (",l", po::value<vector<string>>(&poLog), "Activate execution logs, for valid options see below")
    ;
}


#define INITIALISE_FAIL success = false;	\
  goto initialise_end;

bool initialise(vector<Task*>& taskset) {
  cout << "Initialising program..." << endl;
  bool success = true;

  // Schedulers
  const char* str = poSchedulers.c_str();
  while (*str != '\0') {
    switch (*str) {
    case 'b':
      schedulers.push_back(new BEEDFScheduler());
      break;
    case 'd':
      schedulers.push_back(new DVDEDFScheduler());
      break;
    case 'e':
      schedulers.push_back(new EDFScheduler());
      break;
    case 'f':
      schedulers.push_back(new FPPScheduler());
      break;
    case 'g':
      schedulers.push_back(new GDPAScheduler());
      break;
    case 'h':
      schedulers.push_back(new HCEDFScheduler());
      break;
    case 'm':
      schedulers.push_back(new MKUEDFScheduler());
      break;
    case 'p':
      schedulers.push_back(new PHCEDFScheduler());
      break;
    case 's':
      schedulers.push_back(new GDPASScheduler());
      break;
    }
    str++;
  }

  // Logger
  for (string logClass: poLog) {
    logger::activateClass(logClass);
  }
  cout << "Log class: " << hex << logger::getCurrentClass() << dec << endl;

  // Taskset
  TasksetReader* readerPtr = TasksetReader::getInstance();
  // task and ntasks called by reference
  if (!readerPtr->read(theInputFile, taskset)) {
    cerr << "Error while retrieving taskset from " << theInputFile << "!" << endl;
    INITIALISE_FAIL;
  }
  

 initialise_end:
  return success;
}


void printLegend() {
  cout << "Job: J[taskref],[jobnum] ([remET]/[ET]-[ct] P [priority] S [st])\n";
}


void printUsage(char *prog) {
  cout << "Usage: " << prog << " [arguments] \nArguments:" << endl;
  /*
    TODO: nicer output, e.g. with output class from 
    http://www.radmangames.com/programming/how-to-use-boost-program_options
  */
  cout << desc << "\n";

  cout << "Valid parameters for the -l option are:";
  for (size_t i = 0; i < LOG_N_PREFIX; ++i) {
    cout << " " << tmssim::detail::LOG_PREFIX[i];
  }
  cout << endl;

  cout << "The -l option may be specified multiple times." << endl;
}
  

void calcStat(const vector<int>& v) {
  double sum = 0;
  int n = 0;
  int sd = 1; // successive delays
  size_t max = 0;
  for (vector<int>::const_iterator it = v.begin(); it != v.end(); ++it, ++sd) {
    if (*it != 0) {
      sum += *it * sd;
      n += *it; //sd;
      max = sd;
    }
  }
  double mu = sum / n;
  
  sum = 0;
  sd = 1;
  for (vector<int>::const_iterator it = v.begin(); it != v.end(); ++it, ++sd) {
    if (*it != 0) {
      double tmp = *it * sd - mu;
      sum += tmp * tmp;
      n += *it; //sd;
    }
  }
  
  double var = sum / n;
  
  //cout << "MU: " << mu << " VAR: " << var << " MAX: " << max;
  cout << "MU: " << mu << " SIG: " << sqrt(var) << " MAX: " << max;
}

/**
 * Evaluates the retrieved statistics
 * @param statistics The statistics to be compared to each other
 */
void EvaluateStatistics(vector<SimulationResults>& statistics) {
  cout << endl;
  cout << "########################################" << endl;
  cout << "Evaluating Statistics of all Simulations" << endl;
  cout << "########################################" << endl;
  cout << endl;
  for(size_t i=0; i<statistics.size(); i++) {
    cout << "Did Simulation with Scheduler: " << statistics[i].schedulerId << endl;
    cout << "   Here come the statistics:" << endl;
    cout << statistics[i] << endl;
  }
  //cout << "Leaving out further comparisons..." << endl;
  //cout << "Finished evaluation" << endl;
}
